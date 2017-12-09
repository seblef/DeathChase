
#ifndef _TREEPLACEMENT_H_
#define _TREEPLACEMENT_H_

#include <string>
#include <vector>


class TreePlacement
{
	public:

		TreePlacement() {}
		~TreePlacement() {}

		void Clear();
		void ClearMap();

		void AddTreeTemplate(const char *szName, int iMin, int iMax, float fScale=1.0f);
		bool Generate(int iCount);

	protected:

		class TreeTemplate
		{
			public:

				TreeTemplate() {}
				TreeTemplate(std::string _strName, int _iMin, int _iMax, float _fScale) :
				  strName(_strName),iMin(_iMin),iMax(_iMax),fScale(_fScale) {}
				TreeTemplate(const TreeTemplate& t) : 
				  strName(t.strName),iMin(t.iMin),iMax(t.iMax),fScale(t.fScale) {}
				~TreeTemplate() {}

				std::string strName;
				int iMin;
				int iMax;
				float fScale;
		};

		std::vector<TreeTemplate> m_Templates;

};

#endif