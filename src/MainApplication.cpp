#include "MainApplication.h"
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <functional>
#include <thread>
#include <atomic>

#include "IO/FileDialog.h"
#include "UI/VolumeViewWindow.h"
#include "UI/VolumeViewWindow3D.h"

namespace vr
{
    void MainApplication::ProcessMouseClick()
    {   
        m_leftMouseBtn.HandleInput(m_window);
    }

    void MainApplication::Init()
    {

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

        m_window = glfwCreateWindow(800, 600, "Volume Visualization", nullptr, nullptr);

        if (m_window == nullptr)
            throw std::runtime_error("Error: Could not initialize GLFW window");

        glfwMakeContextCurrent(m_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw std::runtime_error("Error: Could not initialize GLAD");

        std::cout << "GL_VERSION:  " << glGetString(GL_VERSION) << '\n'
                  << "GL_RENDERER: " << glGetString(GL_RENDERER) << '\n'
                  << "GL_VENDOR:   " << glGetString(GL_VENDOR) << '\n';
        
        // Initialize ImGUI
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // ImGui::StyleColorsLight();
        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init("#version 450");

        m_viewport3d.Initialize();
        for (VolumeViewWindow2D& view : m_crossSectionViews)
        {
            view.Initialize();
        }
    }

    void MainApplication::RunMainLoop()
    {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar               | ImGuiWindowFlags_NoDocking   |
                                       ImGuiWindowFlags_NoTitleBar            | ImGuiWindowFlags_NoCollapse  |
                                       ImGuiWindowFlags_NoResize              | ImGuiWindowFlags_NoMove      |
                                       ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        ImGuiIO& io = ImGui::GetIO();

        io.FontDefault = io.Fonts->AddFontFromFileTTF("res/fonts/Lato/Lato-Regular.ttf", 18);

        bool willOpenMetadataDlg = false;
        bool isProcessingFile    = false;
        std::string filePath = "";

        while (!glfwWindowShouldClose(m_window))
        {
            glfwPollEvents();

            ProcessMouseClick();

            m_viewport3d.ProcessInput(m_window);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            glClearColor(0.3f, 0.1f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Set styling parameters for the root dockspace
            ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

            ImGui::Begin("RootDockspace", nullptr, windowFlags);
            ImGui::PopStyleVar(3);

            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspaceId = ImGui::GetID("RootDockspace");
                ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);
            }

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Open", "Ctrl+O"))
                    {
                        m_volumeFilePath = OpenFileDialog(m_window, "RAW file (*.raw)\0*.raw\0\0");
                        if(!m_volumeFilePath.empty())
                            willOpenMetadataDlg = true;
                    }
                    
                    if (ImGui::MenuItem("Quit", "Ctrl+Q"))
                        glfwSetWindowShouldClose(m_window, GLFW_TRUE);

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Options"))
                {
                    ImGui::MenuItem("Settings");
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Help"))
                {
                    ImGui::MenuItem("About");
                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            if (willOpenMetadataDlg)
            {
                ImGui::OpenPopup("Dataset Properties");
                willOpenMetadataDlg = false;
            }

            ShowMetadataModal();

            if (m_loadDatasetTask.IsInProgress())
            {
                ImGui::OpenPopup("Loading");

                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                if (ImGui::BeginPopupModal("Loading", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("Loading the dataset. This might take a while...");
                    ImGui::EndPopup();
                }
            }
            if (m_loadDatasetTask.CheckIfFinished())
            {
                if (!m_loadDatasetTask.HasRetrieved())
                {
                    auto res  = m_loadDatasetTask.GetResult();
                    if (res.has_value())
                    {
                        VolumeDataset dataset = std::move(res.value());
                        GLenum dataType = 0;
                        switch (dataset.DataType())
                        {
                            case VolumeDataType::UINT8:  dataType = GL_UNSIGNED_BYTE ; break;
                            case VolumeDataType::UINT16: dataType = GL_UNSIGNED_SHORT; break;
                            case VolumeDataType::FLOAT : dataType = GL_FLOAT;          break;
                            case VolumeDataType::DOUBLE: dataType = GL_DOUBLE;         break;
                            default: assert(false); break;
                        }

                        TextureProperties props;
                        props.width       = dataset.DataSize()[0];
                        props.height      = dataset.DataSize()[1];
                        props.depth       = dataset.DataSize()[2];
                        props.target      = GL_TEXTURE_3D;
                        props.datatype    = dataType;
                        props.minFilter   = GL_LINEAR_MIPMAP_NEAREST;
                        props.magFilter   = GL_LINEAR;
                        props.wrappingS   = GL_CLAMP_TO_BORDER;
                        props.wrappingT   = GL_CLAMP_TO_BORDER;
                        props.wrappingR   = GL_CLAMP_TO_BORDER;
                        props.fmtInternal = GL_RED;
                        props.fmtSource   = GL_RED;
                        props.areMipmapsEnabled = true;
                        
                        Texture texture3d { dataset.RawData(), props };

                        m_volumeModel.reset(new VolumeModel(std::move(dataset), std::move(texture3d)));
                        
                        TransferFunction1D tf = {};
                        tf.AddColorStop(0.00f, 0.0f, 0.8f, 1.0f);
                        tf.AddColorStop(1.00f, 0.8f, 1.0f, 0.0f);
                        tf.AddOpacityStop(0.00f, 0.00f);
                        tf.AddOpacityStop(0.25f, 0.00f);
                        tf.AddOpacityStop(1.00f, 1.00f);
                        
                        tf.CreateTransferFunctionTexture();

                        m_volumeModel->AddTransferFunction("GreenRed", tf);

                        // Update the volume model of the viewports
                        m_viewport3d.UpdateVolumeModel(m_volumeModel.get());
                        m_viewport3d.Refresh();
                        for (VolumeViewWindow2D& view : m_crossSectionViews)
                        {
                            view.UpdateVolumeModel(m_volumeModel.get());
                            view.Refresh();
                        }
                    }
                    else
                    {
                        ImGui::OpenPopup("Error");
                    }
                    m_loadDatasetTask = {};
                }
            }

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Invalid dataset size specified.");
                if (ImGui::Button("Ok"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            ImGui::Begin("Dataset");
            {
                if (m_volumeModel)
                {
                    if (ImGui::BeginTable("datasetDims", 2))
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Slice width");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%d", m_volumeModel->GetDataSize()[0]);
                        
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Slice height");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%d", m_volumeModel->GetDataSize()[1]);

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Total slices");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%d", m_volumeModel->GetDataSize()[2]);

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Spacing X");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%.3f", m_volumeModel->GetDataSpacing()[0]);

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Spacing X");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%.3f", m_volumeModel->GetDataSpacing()[1]);

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Spacing X");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%.3f", m_volumeModel->GetDataSpacing()[2]);

                        ImGui::EndTable();
                    }
                }
                else
                {
                    ImGui::Text("No dataset is currently loaded.");
                }
            }
            ImGui::End();

            //ImGui::ShowDemoWindow();

            ImGui::Begin("Render Settings");
            {
                ImVec2 availArea = ImGui::GetContentRegionAvail();

                static int inputRayFunctionIdx = 0;
                static int prevInputRayFunctionIdx = inputRayFunctionIdx;

                static float firstHitThreshold = 1.0f;
                static float prevFirstHitThreshold = prevFirstHitThreshold;

                static const char* rayFunctionChoices[] = { 
                    "Maximum intensity projection", 
                    "Average intensity projection", 
                    "First-hit", 
                    "Composite"
                };

                static const ProjectionMethod rayFunctionVals[] = { 
                    ProjectionMethod::MAX_INTENSITY,
                    ProjectionMethod::AVG_INTENSITY,
                    ProjectionMethod::FIRST_HIT,
                    ProjectionMethod::COMPOSITE,
                };

                if (m_volumeModel)
                {
                    ImGui::Combo("Ray function", &inputRayFunctionIdx, rayFunctionChoices, IM_ARRAYSIZE(rayFunctionChoices));

                    if (rayFunctionVals[inputRayFunctionIdx] == ProjectionMethod::FIRST_HIT)
                        ImGui::SliderFloat("Threshold", &firstHitThreshold, 0.0f, 1.0f);

                    ImGui::Text("Transfer Function");
                    ImGui::Image(
                        (ImTextureID) m_volumeModel->GetTransferTextureID("GreenRed"),
                        ImVec2(availArea.x, 32),
                        ImVec2(0, 1),
                        ImVec2(1, 0),
                        ImVec4(1, 1, 1, 1),
                        ImVec4(0, 0, 0, 1)
                    );

                    if (inputRayFunctionIdx != prevInputRayFunctionIdx || firstHitThreshold != prevFirstHitThreshold)
                    {
                        m_viewport3d.UpdateProjectionMethod(rayFunctionVals[inputRayFunctionIdx], firstHitThreshold);
                        prevInputRayFunctionIdx = inputRayFunctionIdx;
                        prevFirstHitThreshold   = firstHitThreshold;
                    }
                    
                }
                else
                {
                    ImGui::Text("No dataset is currently loaded.");
                }
            }
            ImGui::End();

            // Debug window
#ifdef _DEBUG
            ImGui::Begin("Debug Info");
            {
                ImGui::Text("GL_VERSION : %s", glGetString(GL_VERSION));
                ImGui::Text("Frames/sec : %.2f", ImGui::GetIO().Framerate);
            }
            ImGui::End();
#endif

            // Main viewports
            m_viewport3d.RenderUI(m_window);
            for (VolumeViewWindow2D& view : m_crossSectionViews)
            {
                view.RenderUI(m_window);
            }
            
            ImGui::End(); // dockspace

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
            glfwSwapBuffers(m_window);  
        }

    }


    void MainApplication::ShowMetadataModal()
    {

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Dataset Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Enter additional information about the dataset.\n\n");
            ImGui::Separator();

            static const char* items[] = { 
                "8-bit unsigned integer", 
                "16-bit unsigned integer", 
                "32-bit float (IEEE-754)", 
            };

            ImGui::InputScalarN("Dimensions (Width, height, depth)", ImGuiDataType_U32, (void*)m_inputDimensions.data(), 3);
            ImGui::InputScalarN("Spacing (X, Y, Z)", ImGuiDataType_Double, (void*)m_inputSpacings.data(), 3);
            ImGui::Combo("Data type", &m_inputTypeIndex, items, IM_ARRAYSIZE(items));

            ImGui::Separator();

            bool isOkClicked = ImGui::Button("OK", ImVec2(120, 0));
            if (isOkClicked)
            {
                AsyncLoadDataset();
                ImGui::CloseCurrentPopup();
            }

            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) 
            { 
                ImGui::CloseCurrentPopup(); 
            }
            ImGui::EndPopup();
        }
    }

    void MainApplication::AsyncLoadDataset()
    {
        static const VolumeDataType types[] = {
            VolumeDataType::UINT8, VolumeDataType::UINT16,
            VolumeDataType::FLOAT,
        };

        m_loadDatasetTask.Dispatch([&]() -> std::optional<VolumeDataset> {
            try {
                return std::move(VolumeDataset(m_volumeFilePath,
                                               types[m_inputTypeIndex],
                                               m_inputDimensions,
                                               m_inputSpacings));
            }
            catch (std::exception& e) {
                std::cerr << e.what() << '\n';
                return {};
            }
            
        });
    }

    MainApplication::MainApplication()
    {
        Init();
    }

    MainApplication::~MainApplication()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}
