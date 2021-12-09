/* Script Class Implementation */
#include "Script.h"

#include "GameObject.h"

namespace duckApp
{
	/* TODO (1.12) Implement Constructor That Initializes Script File Instance */
	Script::Script(std::string filePath) :scriptFile{ filePath }
	{
	}

	Script::~Script()
	{
	}


	/* TODO (1.13) Implement Initialize Method */
	/*
		* Sets Path Variable (Environment) To Contain Current Script Location (Catalog)
		* Imports Script Module
		* Gets Class From The Script Implementation
		* Constructs New Object Of The Given Script Class
		* Gets All Implemented Methods From Script Implementation
		* Sets Given Game Object As Class Member
	*/
	void Script::Initialize(GameObject* gameObject)
	{
		PySys_SetPath(std::wstring(this->scriptFile.scriptPath.begin(), this->scriptFile.scriptPath.end()).c_str());

		py::module pyModule = py::module::import(this->scriptFile.scriptName.c_str());
		if (pyModule)
		{
			this->pyClassObject = pyModule.attr(this->scriptFile.scriptName.c_str())();
			if (this->pyClassObject)
			{
				this->pyClassObject.attr("gameObject") = gameObject;

				this->onCreateMethod = this->pyClassObject.attr("OnCreate");
				this->onUpdateMethod = this->pyClassObject.attr("OnUpdate");
			}
		}
	}


	/* TODO (1.14) Implement FileChanged Method That Checks If File Has Changed */
	bool Script::FileChanged()
	{
		return scriptFile.fileChanged();
	}


	/* TODO (1.15) Implement OnUpdate Method That Calls Python OnUpdate Sctipt Mathod */
	void Script::OnUpdate()
	{
		if (this->onUpdateMethod)
		{
			this->onUpdateMethod();
		}
	}


	/* TODO (1.16) Implement OnUCreate Method That Calls Python OnCreate Sctipt Mathod */
	void Script::OnCreate()
	{
		if (this->onCreateMethod)
		{
			this->onCreateMethod();
		}
	}
}
