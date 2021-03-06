REM: A batch program to rebuild the Win32++ samples using VS2008 Express
REM: The contents of the log file is erased 

REM: Set the Paths
@set PATH=c:\Program Files\Microsoft Visual Studio 9.0\Common7\IDE;c:\Program Files\Microsoft Visual Studio 9.0\VC\BIN;c:\Program Files\Microsoft Visual Studio 9.0\Common7\output;C:\Program Files\Microsoft Visual Studio 9.0\VC\VCPackages;c:\WINDOWS\Microsoft.NET\Framework\v3.5;c:\WINDOWS\Microsoft.NET\Framework\v2.0.50727;%PATH%
@set PATH=c:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\IDE;c:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\BIN;c:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\output;C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\VCPackages;c:\WINDOWS\Microsoft.NET\Framework\v3.5;c:\WINDOWS\Microsoft.NET\Framework\v2.0.50727;%PATH%



::Cleanup
call clean
call CleanTutorials
pushd "..\..\Networking"
call clean
popd


REM: Change the directory to the parent
pushd ..\..
if exist "Win32++\output\VS2008\Debug" rmdir /s /q "Win32++\output\VS2008\Debug"
if exist "Win32++\output\VS2008\Release" rmdir /s /q "Win32++\output\VS2008\Release"
if exist "Win32++\output\VS2008\Tutorials" rmdir /s /q "Win32++\output\VS2008\Tutorials"


::Compile code
vcbuild /rebuild Browser\ProjectFiles\Browser_2008.vcproj debug	                > "Win32++\output\VS2008.log"
vcbuild /rebuild Browser\ProjectFiles\Browser_2008.vcproj release	        >>"Win32++\output\VS2008.log"
vcbuild /rebuild Dialog\ProjectFiles\Dialog_2008.vcproj debug			>>"Win32++\output\VS2008.log"
vcbuild /rebuild Dialog\ProjectFiles\Dialog_2008.vcproj release			>>"Win32++\output\VS2008.log"
vcbuild /rebuild DialogBars\ProjectFiles\DialogBars_2008.vcproj debug	        >>"Win32++\output\VS2008.log"
vcbuild /rebuild DialogBars\ProjectFiles\DialogBars_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild DialogDemo\ProjectFiles\DialogDemo_2008.vcproj debug		>>"Win32++\output\VS2008.log"
vcbuild /rebuild DialogDemo\ProjectFiles\DialogDemo_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild DialogTab\ProjectFiles\DialogTab_2008.vcproj debug		>>"Win32++\output\VS2008.log"
vcbuild /rebuild DialogTab\ProjectFiles\DialogTab_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild DirectX\ProjectFiles\DirectX_2008.vcproj debug			>>"Win32++\output\VS2008.log"
vcbuild /rebuild DirectX\ProjectFiles\DirectX_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Dock\ProjectFiles\Dock_2008.vcproj debug			>>"Win32++\output\VS2008.log"
vcbuild /rebuild Dock\ProjectFiles\Dock_2008.vcproj release			>>"Win32++\output\VS2008.log"
vcbuild /rebuild DockContainer\ProjectFiles\DockContainer_2008.vcproj debug	>>"Win32++\output\VS2008.log"
vcbuild /rebuild DockContainer\ProjectFiles\DockContainer_2008.vcproj release	>>"Win32++\output\VS2008.log"
vcbuild /rebuild DockTabbedMDI\ProjectFiles\DockTabbedMDI_2008.vcproj debug	>>"Win32++\output\VS2008.log"
vcbuild /rebuild DockTabbedMDI\ProjectFiles\DockTabbedMDI_2008.vcproj release	>>"Win32++\output\VS2008.log"
vcbuild /rebuild Explorer\ProjectFiles\Explorer_2008.vcproj debug		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Explorer\ProjectFiles\Explorer_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild FastGDI\ProjectFiles\FastGDI_2008.vcproj debug			>>"Win32++\output\VS2008.log"
vcbuild /rebuild FastGDI\ProjectFiles\FastGDI_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild FormDemo\ProjectFiles\FormDemo_2008.vcproj debug		>>"Win32++\output\VS2008.log"
vcbuild /rebuild FormDemo\ProjectFiles\FormDemo_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Frame\ProjectFiles\Frame_2008.vcproj debug			>>"Win32++\output\VS2008.log"
vcbuild /rebuild Frame\ProjectFiles\Frame_2008.vcproj release			>>"Win32++\output\VS2008.log"
vcbuild /rebuild MDIFrame\ProjectFiles\MDIFrame_2008.vcproj debug		>>"Win32++\output\VS2008.log"
vcbuild /rebuild MDIFrame\ProjectFiles\MDIFrame_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild MDIFrameDemo\ProjectFiles\MDIDemo_2008.vcproj debug		>>"Win32++\output\VS2008.log"
vcbuild /rebuild MDIFrameDemo\ProjectFiles\MDIDemo_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Networking\ClientDlg\Client_2008.vcproj debug			>>"Win32++\output\VS2008.log"
vcbuild /rebuild Networking\ClientDlg\Client_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Networking\ServerDlg\Server_2008.vcproj debug			>>"Win32++\output\VS2008.log"
vcbuild /rebuild Networking\ServerDlg\Server_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild NotePad\ProjectFiles\Notepad_2008.vcproj debug			>>"Win32++\output\VS2008.log"
vcbuild /rebuild NotePad\ProjectFiles\Notepad_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Performance\ProjectFiles\Performance_2008.vcproj debug		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Performance\ProjectFiles\Performance_2008.vcproj release	>>"Win32++\output\VS2008.log"
vcbuild /rebuild Picture\ProjectFiles\Picture_2008.vcproj debug			>>"Win32++\output\VS2008.log"
vcbuild /rebuild Picture\ProjectFiles\Picture_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild PropertySheet\ProjectFiles\PropertySheet_2008.vcproj debug	>>"Win32++\output\VS2008.log"
vcbuild /rebuild PropertySheet\ProjectFiles\PropertySheet_2008.vcproj release	>>"Win32++\output\VS2008.log"
vcbuild /rebuild Scribble\ProjectFiles\Scribble_2008.vcproj debug		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Scribble\ProjectFiles\Scribble_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Simple\ProjectFiles\Simple_2008.vcproj debug			>>"Win32++\output\VS2008.log"
vcbuild /rebuild Simple\ProjectFiles\Simple_2008.vcproj release			>>"Win32++\output\VS2008.log"
vcbuild /rebuild Splitter\ProjectFiles\Splitter_2008.vcproj debug		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Splitter\ProjectFiles\Splitter_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild RibbonFrame\ProjectFiles\RibbonFrame_2008.vcproj debug		>>"Win32++\output\VS2008.log"
vcbuild /rebuild RibbonFrame\ProjectFiles\RibbonFrame_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild RibbonSimple\ProjectFiles\SimpleRibbon_2008.vcproj debug		>>"Win32++\output\VS2008.log"
vcbuild /rebuild RibbonSimple\ProjectFiles\SimpleRibbon_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild TabDemo\ProjectFiles\TabDemo_2008.vcproj debug			>>"Win32++\output\VS2008.log"
vcbuild /rebuild TabDemo\ProjectFiles\TabDemo_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Themes\ProjectFiles\Themes_2008.vcproj debug			>>"Win32++\output\VS2008.log"
vcbuild /rebuild Themes\ProjectFiles\Themes_2008.vcproj release			>>"Win32++\output\VS2008.log"
vcbuild /rebuild Threads\ProjectFiles\Threads_2008.vcproj debug			>>"Win32++\output\VS2008.log"
vcbuild /rebuild Threads\ProjectFiles\Threads_2008.vcproj release		>>"Win32++\output\VS2008.log"

mkdir "Win32++\output\VS2008"
mkdir "Win32++\output\VS2008\Debug"

ECHO "Copying Debug Samples" >>"Win32++\output\VS2008.log"
copy Browser\ProjectFiles\Debug\Browser.exe                 "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy Dialog\ProjectFiles\Debug\Dialog.exe                   "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy DialogBars\ProjectFiles\Debug\DialogBars.exe           "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy DialogDemo\ProjectFiles\Debug\DialogDemo.exe           "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy DialogTab\ProjectFiles\Debug\DialogTab.exe             "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy DirectX\ProjectFiles\Debug\DirectX.exe                 "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy Dock\ProjectFiles\Debug\Dock.exe                       "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy DockContainer\ProjectFiles\Debug\DockContainer.exe     "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy DockTabbedMDI\ProjectFiles\Debug\DockTabbedMDI.exe     "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy Explorer\ProjectFiles\Debug\Explorer.exe               "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy FastGDI\ProjectFiles\Debug\FastGDI.exe                 "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy FormDemo\ProjectFiles\Debug\FormDemo.exe               "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy Frame\ProjectFiles\Debug\Frame.exe                     "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy MDIFrame\ProjectFiles\Debug\MDIFrame.exe               "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy MDIFrameDemo\ProjectFiles\Debug\MDIDemo.exe            "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy Networking\ClientDlg\Debug\Client.exe                  "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy Networking\ServerDlg\Debug\Server.exe                  "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy NotePad\ProjectFiles\Debug\Notepad.exe                 "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy Performance\ProjectFiles\Debug\Performance.exe         "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy Picture\ProjectFiles\Debug\Picture.exe                 "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy PropertySheet\ProjectFiles\Debug\PropertySheet.exe     "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy RibbonFrame\ProjectFiles\Debug\RibbonFrame.exe         "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy RibbonSimple\ProjectFiles\Debug\SimpleRibbon.exe       "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy Scribble\ProjectFiles\Debug\Scribble.exe               "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy Simple\ProjectFiles\Debug\Simple.exe                   "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy Splitter\ProjectFiles\Debug\Splitter.exe               "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy TabDemo\ProjectFiles\Debug\TabDemo.exe                 "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy Themes\ProjectFiles\Debug\Themes.exe                   "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"
copy Threads\ProjectFiles\Debug\Threads.exe                 "Win32++\output\VS2008\Debug"    >>"Win32++\output\VS2008.log"

mkdir "Win32++\output\VS2008\Release"

ECHO "Copying Release Samples" >>"Win32++\output\VS2008.log"
copy Browser\ProjectFiles\Release\Browser.exe                 "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy Dialog\ProjectFiles\Release\Dialog.exe                   "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy DialogBars\ProjectFiles\Release\DialogBars.exe           "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy DialogDemo\ProjectFiles\Release\DialogDemo.exe           "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy DialogTab\ProjectFiles\Release\DialogTab.exe             "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy DirectX\ProjectFiles\Release\DirectX.exe                 "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy Dock\ProjectFiles\Release\Dock.exe                       "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy DockContainer\ProjectFiles\Release\DockContainer.exe     "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy DockTabbedMDI\ProjectFiles\Release\DockTabbedMDI.exe     "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy Explorer\ProjectFiles\Release\Explorer.exe               "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy FastGDI\ProjectFiles\Release\FastGDI.exe                 "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy FormDemo\ProjectFiles\Release\FormDemo.exe               "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy Frame\ProjectFiles\Release\Frame.exe                     "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy MDIFrame\ProjectFiles\Release\MDIFrame.exe               "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy MDIFrameDemo\ProjectFiles\Release\MDIDemo.exe            "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy Networking\ClientDlg\Release\Client.exe                  "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy Networking\ServerDlg\Release\Server.exe                  "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy NotePad\ProjectFiles\Release\Notepad.exe                 "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy Performance\ProjectFiles\Release\Performance.exe         "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy Picture\ProjectFiles\Release\Picture.exe                 "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy PropertySheet\ProjectFiles\Release\PropertySheet.exe     "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy RibbonFrame\ProjectFiles\Release\RibbonFrame.exe         "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy RibbonSimple\ProjectFiles\Release\SimpleRibbon.exe       "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy Scribble\ProjectFiles\Release\Scribble.exe               "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy Simple\ProjectFiles\Release\Simple.exe                   "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy Splitter\ProjectFiles\Release\Splitter.exe               "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy TabDemo\ProjectFiles\Release\TabDemo.exe                 "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy Themes\ProjectFiles\Release\Themes.exe                   "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"
copy Threads\ProjectFiles\Release\Threads.exe                 "Win32++\output\VS2008\Release"    >>"Win32++\output\VS2008.log"


REM: Compile Tutorials
ECHO "Compliling Tutorials" >>"Win32++\output\VS2008.log"
::Compile code
vcbuild /rebuild Tutorials\Tutorial1\Tutorial1_2008.vcproj debug 		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Tutorials\Tutorial1\Tutorial1_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Tutorials\Tutorial2\Tutorial2_2008.vcproj debug		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Tutorials\Tutorial2\Tutorial2_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Tutorials\Tutorial3\Tutorial3_2008.vcproj debug		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Tutorials\Tutorial3\Tutorial3_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Tutorials\Tutorial4\Tutorial4_2008.vcproj debug		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Tutorials\Tutorial4\Tutorial4_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Tutorials\Tutorial5\Tutorial5_2008.vcproj debug		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Tutorials\Tutorial5\Tutorial5_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Tutorials\Tutorial6\Tutorial6_2008.vcproj debug		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Tutorials\Tutorial6\Tutorial6_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Tutorials\Tutorial7\Tutorial7_2008.vcproj debug		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Tutorials\Tutorial7\Tutorial7_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Tutorials\Tutorial8\Tutorial8_2008.vcproj debug		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Tutorials\Tutorial8\Tutorial8_2008.vcproj release		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Tutorials\Tutorial9\Tutorial9_2008.vcproj debug		>>"Win32++\output\VS2008.log"
vcbuild /rebuild Tutorials\Tutorial9\Tutorial9_2008.vcproj release		>>"Win32++\output\VS2008.log"

mkdir "Win32++\output\VS2008\Tutorials"
mkdir "Win32++\output\VS2008\Tutorials\Debug"

ECHO "Copying Debug Tutorials" >>"Win32++\output\VS2008.log"
copy Tutorials\Tutorial1\Debug\Tutorial1.exe         "Win32++\output\VS2008\Tutorials\Debug"    >>"Win32++\output\VS2008.log"
copy Tutorials\Tutorial2\Debug\Tutorial2.exe         "Win32++\output\VS2008\Tutorials\Debug"    >>"Win32++\output\VS2008.log"
copy Tutorials\Tutorial3\Debug\Tutorial3.exe         "Win32++\output\VS2008\Tutorials\Debug"    >>"Win32++\output\VS2008.log"
copy Tutorials\Tutorial4\Debug\Tutorial4.exe         "Win32++\output\VS2008\Tutorials\Debug"    >>"Win32++\output\VS2008.log"
copy Tutorials\Tutorial5\Debug\Tutorial5.exe         "Win32++\output\VS2008\Tutorials\Debug"    >>"Win32++\output\VS2008.log"
copy Tutorials\Tutorial6\Debug\Tutorial6.exe         "Win32++\output\VS2008\Tutorials\Debug"    >>"Win32++\output\VS2008.log"
copy Tutorials\Tutorial7\Debug\Tutorial7.exe         "Win32++\output\VS2008\Tutorials\Debug"    >>"Win32++\output\VS2008.log"
copy Tutorials\Tutorial8\Debug\Tutorial8.exe         "Win32++\output\VS2008\Tutorials\Debug"    >>"Win32++\output\VS2008.log"
copy Tutorials\Tutorial9\Debug\Tutorial9.exe         "Win32++\output\VS2008\Tutorials\Debug"    >>"Win32++\output\VS2008.log"

mkdir "Win32++\output\VS2008\Tutorials\Release"

ECHO "Copying Release Tutorials" >>"Win32++\output\VS2008.log"
copy Tutorials\Tutorial1\Release\Tutorial1.exe         "Win32++\output\VS2008\Tutorials\Release"    >>"Win32++\output\VS2008.log"
copy Tutorials\Tutorial2\Release\Tutorial2.exe         "Win32++\output\VS2008\Tutorials\Release"    >>"Win32++\output\VS2008.log"
copy Tutorials\Tutorial3\Release\Tutorial3.exe         "Win32++\output\VS2008\Tutorials\Release"    >>"Win32++\output\VS2008.log"
copy Tutorials\Tutorial4\Release\Tutorial4.exe         "Win32++\output\VS2008\Tutorials\Release"    >>"Win32++\output\VS2008.log"
copy Tutorials\Tutorial5\Release\Tutorial5.exe         "Win32++\output\VS2008\Tutorials\Release"    >>"Win32++\output\VS2008.log"
copy Tutorials\Tutorial6\Release\Tutorial6.exe         "Win32++\output\VS2008\Tutorials\Release"    >>"Win32++\output\VS2008.log"
copy Tutorials\Tutorial7\Release\Tutorial7.exe         "Win32++\output\VS2008\Tutorials\Release"    >>"Win32++\output\VS2008.log"
copy Tutorials\Tutorial8\Release\Tutorial8.exe         "Win32++\output\VS2008\Tutorials\Release"    >>"Win32++\output\VS2008.log"
copy Tutorials\Tutorial9\Release\Tutorial9.exe         "Win32++\output\VS2008\Tutorials\Release"    >>"Win32++\output\VS2008.log"

REM: Pop the directory change off the stack
popd


