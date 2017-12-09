
#ifndef _IEXPORTOBJ_H_
#define _IEXPORTOBJ_H_

enum ExportObjectType
{
	EOBJ_BMPIMAGE=0,
	EOBJ_CAMERA,
	EOBJ_SCRIPTFILE,
	EOBJ_VIEWFRUSTRUM,
	EOBJ_SPACESHIP,

	EOBJ_COUNT

};

const char* const ExportObjectClassNames[]=
{
	"ExpBMPImage",
	"ExpCamera",
	"ExpScriptFile",
	"ExpViewFrustrum",
	"ExpSpaceShip"

};

class IExportObject
{
	public:

		IExportObject(ExportObjectType _Type) : m_Type(_Type) {}
		virtual ~IExportObject() {}

		virtual void Clear()=0;

		ExportObjectType GetType() const { return m_Type; }
		const char *GetClassName() const { return ExportObjectClassNames[m_Type]; }

	private:

		ExportObjectType m_Type;
};

#endif

