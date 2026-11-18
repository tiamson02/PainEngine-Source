================================================================================
    MFC 程式庫 : MenuEditor 專案概觀
===============================================================================

AppWizard 為您建立了 MenuEditor 應用程式。此應用程式不只展示
使用 MFC 的基礎用法，它也是個讓您學習如何撰寫應用程式的出發點。

這個檔案包含構成 MenuEditor 應用程式的所有檔案的摘要。

MenuEditor.vcproj
    這是使用應用程式精靈所建立的 VC++ 專案的主專案檔。它包含有關產生此
    檔案的 Visual C++ 版本資訊，以及以應用程式精靈選取的平台、組態和專
    案功能等相關資訊。

MenuEditor.h
    這是應用程式的主標頭檔。它包括其他專案指定的標頭 (包括 Resource.h) 
    和宣告 CMenuEditorApp 應用程式類別。

MenuEditor.cpp
    這是包含應用程式類別 CMenuEditorApp 的主應用程式來源檔。

MenuEditor.rc
    這是此程式使用的所有 Microsoft Windows 資源的清單。它包括圖示、點陣圖
    以及儲存在 RES 子目錄中的游標。您可以在 Microsoft Visual C++ 中直接編
    輯此檔案。專案資源位於 1028 中。

res\MenuEditor.ico
    這是個圖示檔，它被當成應用程式的圖示使用。主資源檔 MenuEditor.rc 
    中包含此圖示。

res\MenuEditor.rc2
    這個檔案包含未經由 Microsoft Visual C++ 編輯的檔案。您必須將所有不可由
    資源編輯器編輯的資源放置在此檔案中。
/////////////////////////////////////////////////////////////////////////////

關於主框架視窗:
    Windows 檔案總管樣式: 此專案將會包含一個類似 Windows 檔案總管 (有兩個框架) 
    的介面。
MainFrm.h, MainFrm.cpp
    這些檔案包含框架類別 CMainFrame，此類別衍生自
    CFrameWnd 並控制所有 SDI 框架功能。
LeftView.h, LeftView.cpp
    這些檔案包含左框架類別 CLeftView，此類別衍生自
    CTreeView。
res\Toolbar.bmp
    這個點陣圖檔用於建立工具列的並排影像。初始的工具列和狀態列是在 
    CMainFrame 類別中建構。請使用資源編輯器編輯此工
    具列點陣圖，然後在 MenuEditor.rc 中更新 IDR_MAINFRAME TOOLBAR 
    陣列以加入工具列按鈕。
/////////////////////////////////////////////////////////////////////////////

AppWizard 建立文件類型及檢視:

MenuEditorDoc.h, MenuEditorDoc.cpp - 此文件
    這些檔案包含您的 CMenuEditorDoc 類別。編輯這些檔案以加入特殊的文件
    資料，然後實作檔案儲存和載入 (透過 CMenuEditorDoc::Serialize)。
MenuEditorView.h, MenuEditorView.cpp - 文件的檢視
    這些檔案包含您的 CMenuEditorView 類別。
    CMenuEditorView 物件用於檢視 CMenuEditorDoc 物件。
/////////////////////////////////////////////////////////////////////////////

其他功能:

ActiveX 控制項
    此應用程式包含使用 ActiveX 控制項的支援。
/////////////////////////////////////////////////////////////////////////////

其他標準檔案:

StdAfx.h, StdAfx.cpp
    這些檔案用於建置名為 MenuEditor.pch 的先行編譯標頭檔 (PCH)，
    以及名為 StdAfx.obj 的先行編譯型別檔。

Resource.h
    這是標準的標頭檔，它定義新資源 ID。
    Microsoft Visual C++ 會讀取並更新此檔案。

/////////////////////////////////////////////////////////////////////////////

其他注意事項:

AppWizard 使用 "TODO:" 來表示您應該加入或自訂的原始程式碼。

如果您的應用程式在共用的 DLL 中使用 MFC，而且此應用程式使用與作業系統不同的
語言，您必須從 Microsoft Visual C++ CD-ROM 中 Win\System 目錄下將相對應的當
地語系化資源 MFC70XXX.DLL 複製到電腦的 system 或 system32 目錄，然後將它重
新命名為 MFCLOC.DLL ("XXX" 代表語言縮寫。例如，MFC70DEU.DLL 包含翻譯為德文的
資源)。如果您並未執行此步驟，應用程式中的某些 UI 項目仍將使用作業系統所用的
語言。

/////////////////////////////////////////////////////////////////////////////
