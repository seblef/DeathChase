================================================================================
    BIBLIOTHÈQUE MICROSOFT FOUNDATION CLASS : Vue d'ensemble du projet SpaceEditor
===============================================================================

L'Assistant Application a créé cette application SpaceEditor pour  
vous.  Cette application décrit les principes de base de l'utilisation de  
Microsoft Foundation Classes et vous permet de créer votre application.

Ce fichier contient un résumé du contenu de chacun des fichiers qui 
constituent votre application SpaceEditor.

SpaceEditor.vcproj
    Il s'agit du fichier projet principal pour les projets VC++ générés à l'aide 
d'un Assistant Application. 
    Il contient les informations sur la version de Visual C++ qui a généré le 
fichier et  
    des informations sur les plates-formes, configurations et fonctionnalités du 
projet sélectionnées avec 
    l'Assistant Application.

SpaceEditor.h
    Il s'agit du fichier d'en-tête principal de l'application.  Il contient 
d'autres 
    en-têtes de projet spécifiques (y compris Resource.h) et déclare la 
    classe d'application CSpaceEditorApp.

SpaceEditor.cpp
    Il s'agit du fichier source principal de l'application qui contient la 
    classe d'application CSpaceEditorApp.

SpaceEditor.rc
    Il s'agit de la liste de toutes les ressources Microsoft Windows que le 
    programme utilise.  Elle comprend les icônes, les bitmaps et les curseurs 
qui sont stockés 
    dans le sous-répertoire RES.  Ce fichier peut être modifié directement dans 
Microsoft 
    Visual C++. Vos ressources de projet sont dans 1036.

res\SpaceEditor.ico
    Il s'agit d'un fichier icône, qui est utilisé comme icône de l'application.  
Cette 
    icône est incluse par le fichier de ressource principal SpaceEditor.rc.

res\SpaceEditor.rc2
    Ce fichier contient les ressources qui ne sont pas modifiées par Microsoft  
    Visual C++. Vous devez placer toutes les ressources non modifiables par 
    l'éditeur de ressources dans ce fichier.

SpaceEditor.reg
    Il s'agit d'un exemple de fichier .reg qui montre le type de paramètres 
    d'enregistrement que le framework définit pour vous.  Vous pouvez l'utiliser 
comme fichier .reg
    pour votre application ou le supprimer et utiliser  
    l'enregistrement par défaut RegisterShellFileTypes.


/////////////////////////////////////////////////////////////////////////////

Pour la fenêtre frame principale :
    Le projet comprend une interface MFC standard.

MainFrm.h, MainFrm.cpp
    Ces fichiers contiennent la classe de frame CMainFrame 
dérivée de
    CFrameWnd et qui contrôle toutes les fonctionnalités des frames SDI.

res\Toolbar.bmp
    Ce fichier bitmap sert à créer des images en mosaïque pour la barre 
d'outils.
    La barre d'outils et la barre d'état initiales sont construites dans la 
classe  
CMainFrame. Modifiez cette image de barre d'outils avec 
l'éditeur de ressources et 
    actualisez le tableau IDR_MAINFRAME TOOLBAR dans SpaceEditor.rc 
pour ajouter 
    les boutons de barre d'outils.
/////////////////////////////////////////////////////////////////////////////

L'Assistant Application crée un type de document et une vue :

SpaceEditorDoc.h, SpaceEditorDoc.cpp - le document
    Ces fichiers contiennent votre classe CSpaceEditorDoc.  Modifiez ces 
fichiers pour 
    ajouter les données de document spéciales et implémenter l'enregistrement et 
le 
    chargement des fichiers (via CSpaceEditorDoc::Serialize).
    Le document contiendra les chaînes suivantes :
        Extension de fichier :      dship
        ID du type de fichier :        SpaceEditor.Document
        Titre du frame principal :  Space Editor
        Nom du type de document :       DCSpaceShip
        Nom de filtre :         DC Space Ship Files (*.dship)
        Nom court de nouveau fichier : SpaceShip
        Nom long du type de fichier : SpaceEditor.Document

SpaceEditorView.h, SpaceEditorView.cpp - la vue du document
    Ces fichiers contiennent votre classe CSpaceEditorView.
    Les objets CSpaceEditorView servent à afficher les objets CSpaceEditorDoc.





/////////////////////////////////////////////////////////////////////////////

Autres fichiers standard :

StdAfx.h, StdAfx.cpp
    Ces fichiers sont utilisés pour générer un fichier d'en-tête précompilé 
(PCH) 
    nommé SpaceEditor.pch et un fichier de type précompilé nommé 
Stdafx.obj.

Resource.h
    Il s'agit du ficher d'en-tête standard, qui définit les nouveaux ID de 
ressources.
    Microsoft Visual C++ lit et met à jour ce fichier.

SpaceEditor.manifest
	Les fichiers manifestes d'application sont utilisés par Windows XP pour 
décrire les dépendances des applications  
	sur des versions spécifiques des assemblys côte à côte. Le chargeur 
utilise ces  
	informations pour charger l'assembly approprié à partir du cache de 
l'assembly ou directement  
	à partir de l'application. Le manifeste de l'application peut être inclus 
pour redistribution  
	comme fichier .manifest externe installé dans le même dossier que 
l'exécutable de l'application  
	ou être inclus dans l'exécutable sous la forme d'une ressource. 
/////////////////////////////////////////////////////////////////////////////

Autres remarques :

L'Assistant Application utilise "TODO:" pour indiquer les parties du code source 
où 
vous devrez ajouter ou modifier du code.

Si votre application utilise les MFC dans une DLL partagée vous devez redistribuer 
les DLL MFC. Si la langue de votre application n'est pas celle du système 
d'exploitation, vous devez également redistribuer le fichier des  ressources 
localisées MFC80XXX.DLL. Pour plus d'informations, consultez la section relative 
à la redistribution des applications Visual C++ dans la documentation MSDN.

/////////////////////////////////////////////////////////////////////////////
