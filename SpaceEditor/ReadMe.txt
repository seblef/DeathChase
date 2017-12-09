================================================================================
    BIBLIOTH�QUE MICROSOFT FOUNDATION CLASS : Vue d'ensemble du projet SpaceEditor
===============================================================================

L'Assistant Application a cr�� cette application SpaceEditor pour  
vous.  Cette application d�crit les principes de base de l'utilisation de  
Microsoft Foundation Classes et vous permet de cr�er votre application.

Ce fichier contient un r�sum� du contenu de chacun des fichiers qui 
constituent votre application SpaceEditor.

SpaceEditor.vcproj
    Il s'agit du fichier projet principal pour les projets VC++ g�n�r�s � l'aide 
d'un Assistant Application. 
    Il contient les informations sur la version de Visual C++ qui a g�n�r� le 
fichier et  
    des informations sur les plates-formes, configurations et fonctionnalit�s du 
projet s�lectionn�es avec 
    l'Assistant Application.

SpaceEditor.h
    Il s'agit du fichier d'en-t�te principal de l'application.  Il contient 
d'autres 
    en-t�tes de projet sp�cifiques (y compris Resource.h) et d�clare la 
    classe d'application CSpaceEditorApp.

SpaceEditor.cpp
    Il s'agit du fichier source principal de l'application qui contient la 
    classe d'application CSpaceEditorApp.

SpaceEditor.rc
    Il s'agit de la liste de toutes les ressources Microsoft Windows que le 
    programme utilise.  Elle comprend les ic�nes, les bitmaps et les curseurs 
qui sont stock�s 
    dans le sous-r�pertoire RES.  Ce fichier peut �tre modifi� directement dans 
Microsoft 
    Visual C++. Vos ressources de projet sont dans 1036.

res\SpaceEditor.ico
    Il s'agit d'un fichier ic�ne, qui est utilis� comme ic�ne de l'application.  
Cette 
    ic�ne est incluse par le fichier de ressource principal SpaceEditor.rc.

res\SpaceEditor.rc2
    Ce fichier contient les ressources qui ne sont pas modifi�es par Microsoft  
    Visual C++. Vous devez placer toutes les ressources non modifiables par 
    l'�diteur de ressources dans ce fichier.

SpaceEditor.reg
    Il s'agit d'un exemple de fichier .reg qui montre le type de param�tres 
    d'enregistrement que le framework d�finit pour vous.  Vous pouvez l'utiliser 
comme fichier .reg
    pour votre application ou le supprimer et utiliser  
    l'enregistrement par d�faut RegisterShellFileTypes.


/////////////////////////////////////////////////////////////////////////////

Pour la fen�tre frame principale :
    Le projet comprend une interface MFC standard.

MainFrm.h, MainFrm.cpp
    Ces fichiers contiennent la classe de frame CMainFrame 
d�riv�e de
    CFrameWnd et qui contr�le toutes les fonctionnalit�s des frames SDI.

res\Toolbar.bmp
    Ce fichier bitmap sert � cr�er des images en mosa�que pour la barre 
d'outils.
    La barre d'outils et la barre d'�tat initiales sont construites dans la 
classe  
CMainFrame. Modifiez cette image de barre d'outils avec 
l'�diteur de ressources et 
    actualisez le tableau IDR_MAINFRAME TOOLBAR dans SpaceEditor.rc 
pour ajouter 
    les boutons de barre d'outils.
/////////////////////////////////////////////////////////////////////////////

L'Assistant Application cr�e un type de document et une vue :

SpaceEditorDoc.h, SpaceEditorDoc.cpp - le document
    Ces fichiers contiennent votre classe CSpaceEditorDoc.  Modifiez ces 
fichiers pour 
    ajouter les donn�es de document sp�ciales et impl�menter l'enregistrement et 
le 
    chargement des fichiers (via CSpaceEditorDoc::Serialize).
    Le document contiendra les cha�nes suivantes :
        Extension de fichier :      dship
        ID du type de fichier :        SpaceEditor.Document
        Titre du frame principal :  Space Editor
        Nom du type de document :       DCSpaceShip
        Nom de filtre :         DC Space Ship Files (*.dship)
        Nom court de nouveau fichier : SpaceShip
        Nom long du type de fichier : SpaceEditor.Document

SpaceEditorView.h, SpaceEditorView.cpp - la vue du document
    Ces fichiers contiennent votre classe CSpaceEditorView.
    Les objets CSpaceEditorView servent � afficher les objets CSpaceEditorDoc.





/////////////////////////////////////////////////////////////////////////////

Autres fichiers standard :

StdAfx.h, StdAfx.cpp
    Ces fichiers sont utilis�s pour g�n�rer un fichier d'en-t�te pr�compil� 
(PCH) 
    nomm� SpaceEditor.pch et un fichier de type pr�compil� nomm� 
Stdafx.obj.

Resource.h
    Il s'agit du ficher d'en-t�te standard, qui d�finit les nouveaux ID de 
ressources.
    Microsoft Visual C++ lit et met � jour ce fichier.

SpaceEditor.manifest
	Les fichiers manifestes d'application sont utilis�s par Windows XP pour 
d�crire les d�pendances des applications  
	sur des versions sp�cifiques des assemblys c�te � c�te. Le chargeur 
utilise ces  
	informations pour charger l'assembly appropri� � partir du cache de 
l'assembly ou directement  
	� partir de l'application. Le manifeste de l'application peut �tre inclus 
pour redistribution  
	comme fichier .manifest externe install� dans le m�me dossier que 
l'ex�cutable de l'application  
	ou �tre inclus dans l'ex�cutable sous la forme d'une ressource. 
/////////////////////////////////////////////////////////////////////////////

Autres remarques :

L'Assistant Application utilise "TODO:" pour indiquer les parties du code source 
o� 
vous devrez ajouter ou modifier du code.

Si votre application utilise les MFC dans une DLL partag�e vous devez redistribuer 
les DLL MFC. Si la langue de votre application n'est pas celle du syst�me 
d'exploitation, vous devez �galement redistribuer le fichier des  ressources 
localis�es MFC80XXX.DLL. Pour plus d'informations, consultez la section relative 
� la redistribution des applications Visual C++ dans la documentation MSDN.

/////////////////////////////////////////////////////////////////////////////
