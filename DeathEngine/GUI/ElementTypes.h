
#ifndef _ELEMENTSTYPE_H_

#define _ELEMENTSTYPE_H_

namespace GUI
{

enum ELEMENTTYPE
{
	ET_BUTTON=0,
	ET_KEYBUTTON,
	ET_CHECKBOX,
	ET_COMBOBOX,
	ET_CONTEXTMENU,
	ET_MENU,
	ET_EDITBOX,
	ET_FILEDIALOG,
	ET_IMAGEFILEDIALOG,
	ET_COLORDIALOG,
	ET_INOUTFADER,
	ET_IMAGE,
	ET_LISTBOX,
	ET_MULTILISTBOX,
	ET_MESHVIEWER,
	ET_MESSAGEBOX,
	ET_MODALSCREEN,
	ET_SCROLLBAR,
	ET_STATICTEXT,
	ET_TAB,
	ET_TABCONTROL,
	ET_TOOLBAR,
	ET_WINDOW,
	ET_SPINBOX,
	ET_COUNT,
	ET_ELEMENT,
	ET_FORCEDWORD=0x7fffffff

};

const char * const ElementTypeNames[]=
{
	"button",
	"keyButton",
	"checkBox",
	"comboBox",
	"contextMenu",
	"menu",
	"editBox",
	"fileDialog",
	"imageFileDialog",
	"colorDialog",
	"inOutFader",
	"image",
	"listBox",
	"multiListBox",
	"meshViewer",
	"messageBox",
	"modalScreen",
	"scrollBar",
	"staticText",
	"tab",
	"tabControl",
	"toolBar",
	"window",
	"spinBox",
	0
};

}

#endif