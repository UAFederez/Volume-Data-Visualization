#include "MainWindow.h"

class MainApplication : public wxApp
{
public:
	virtual bool OnInit()
	{
		m_window = new MainWindow("Volume Visualization");
		m_window->Maximize(true);
		m_window->Show(true);
		return true;
	}

	~MainApplication()
	{

	}
private:
	MainWindow* m_window = nullptr;
};

wxIMPLEMENT_APP(MainApplication);