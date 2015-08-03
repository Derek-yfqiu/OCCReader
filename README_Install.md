********************************************************************************
OCCReader ---
A plugin for visualizing CAD file in STEP,IGES and BREP format in  ParaView. 
********************************************************************************


********************************************************************************
********************  Compile in Linux system **********************************

This compilation process was tested in Ubutun 14.04 system. Remember to change
all the abbreviation (for example $PVBUILD)to the actual folder name.

****Step 1. Download ParaView source and depended dependencies on the plugin. 
    
->Download the ParaView source package from www.paraview.org/download.
Upzip it into a folder (Here brief as $PVSRC): 
	tar zxvf ParaView-v*.*.*-source.tar.gz -C $PVSRC

->Download the SALOME-platfrom. Because this plugin compilation depends on many
open-source toolkit, for example OCC, QT, VTK, meanwhile  these toolkit also
depend on other libraries, a efficient solution is installing SALOME-platform
which includes all these libraries and toolkits in a whole package.

    -> In http://www.salome-platform.org/downloads/current-version, download the
    version which is closest to your system. 
    
    -> Upzip it:
       tar zxvf InstallWizard_*.*.*_Ubuntu_**.**_**bit.tar.gz 
    
    -> run the installation script "runInstall". 
    
    -> Find a folder to install SALOME, here brief as $SALOME.
    
    -> Keep the all the settings by default, only pay attention to installation
    step 5 "Choice of the products be installed". In this page, DESELECT all the
    modules because we don't need them. Then click the "Prerequistes", and
    select "Qt" and "OpenCascade", which are dependencies we needed. Keep
    clicking "NEXT" and finished the installation.



****Step 2. Compile ParaView with the plugin
-> Copy the OCCReader folder to $PVSRC/Plugins, so that we have all the files in 
	$PVSRC/Plugins/OCCReader folder

-> Create a new folder ( Here brief as $PVBUILD) for compiling ParaView: 
	mkdir ParaView-v*.*.*-build.

-> First offer the suitable environment for compiling ParaView: 
	source $SALOME/env_products.sh 

-> Go to the $PVBUILD: 
	cd $PVBUILD
   
-> configure paraview: 
	ccmake $PVSRC

-> Set the CMAKE_INSTALL_PREFIX to a desire installation folder (brief as $PVINSTALL)

-> press "c" to configure, and "g" to generate

-> If errors, check the environment setting. 

-> compile and install the ParaView: 
   make install

You can find a file named "libOCCReader.so" under the $PVINSTALL/lib/paraview-*.* . 



****Step 3. Run ParaView with this plugin

To run ParaView with this plugin, you need to run it under proper environment.

-> set the environment variable: source $SALOME/env_products.sh

-> then run ParaView binary in the $PVINSTALL/bin 

You can write a bash script to make it easier. for example: 

	#!/bin/bash
	source $SALOME/env_products.sh
	$PVINSTALL/bin/paraview

Copy the plugin library "libOCCReader.so" to the $PVINSTALL/lib. You need to load
this library manually for the first time. In ParaView menu Tools->Manage
Plugins, click "Load New" to load the "libOCCReader.so" in the $PVINSTALL/lib/paraview-*.*/, and
select "Auto Load" to load it automatically.



********************************************************************************
********************  Compile in Windows system ********************************


The tested system is Windows7. You need cmake for generating the project, and
Visual Studio for compilation. The current tested version is Visual studio 2010.
Download cmake from http://www.cmake.org/download and choose Win32 installer,
and install it in a folder brief as $CMAKE. Remember to change all the
abbreviation to the actual folder name.
  

****Step 1. Download ParaView source and depended dependencies on the plugin. 
    
->Download the ParaView source package from www.paraview.org/download. Upzip it
into a folder (Here brief as $PVSRC). You need tools like 7zip to unzip the
package.

->Download the SALOME-platfrom. Because this plugin compilation depends on many
open-source toolkit, for example OCC, QT, VTK, meanwhile these toolkits also
depend on other libraries, a efficient solution is installing SALOME-platform
which includes all these libraries and toolkits.

    -> In http://www.salome-platform.org/downloads/current-version, download the
    Windows SDK version of SALOME. The tested version is SALOME_7.4.0_32bit.
    
    ->Extract it in a folder, here brief as $SALOME. 
    
    ->Copy the "compile.bat" under $SALOME\WORK and rename it as
    "compile_env.bat", we are going to use the environment setting in this file
    for compiling ParaView. 
    
        -> Remove the following line from the file "compile_env.bat", since we 
        don't need to run the compilation of SALOME: 
        %PYTHONBIN% compile.py %*


****Step 2. Compile  ParaView with the plugin

->Open a windows dos terminal, change to the $SALOME\WORK folder
      cd  $SALOME\WORK 
      then execute the "compile_env.bat"
    
->In the same terminal, change folder to $CMAKE
      cd $CMAKE
    
->Open the "cmake-gui.exe". 
    
    -> Specify the folder containing the ParaView Source  $PVSRC
    
    -> Create a new folder ( Here brief as $PVBUILD) for compiling ParaView, and
    specify it in cmake. 
    
    -> Click "Configure", choose the Visual Studio version you installed. If you
    are going to build a 64bit version, choose the versions with "Win64". 
    Leave other option by default. NOTED that the OCC library
    in SALOME Windows SDK is 32bit only, which means that you have to choose 32bit
    version. If you decide to build a 64bit version, then you need to download and 
    compile a new OCC libaray 	in 64bit version.
    
    ->If no problem in configuration, then click "Generate" to generated the
    project files.

-> Go to $PVBUILD, you will find a "ALL_BUILD.vcxproj". Open it with Visual
Studio, and build. You can find the compiled ParaView under the
$PVBUILD\bin\Debug(Release).

-> Trouble-shoting in Compilation

	-> You might have the problem in finding OCC libraries in Cmake configuration.
	The OCC change folder structure unregularly, thus you need to pay attention on
	the "IF(WINDOWS)" block of the file "FindCAS.cmake", and modify it manually if
	necessary to help Cmake finding the correct folder of the OCC. In any case,
	you can include this folder in the system environent variable "lib".
		
	-> The environment variable for OCC is "CAS_ROOT_DIR" in SALOME platform, 
		but "CASROOT" in a self-installed OCC library. Check the 
		"SET(CASROOT $ENV{CAS_ROOT_DIR})" at the beginning of the  "FindCAS.cmake", 
		and change it appropriately. 
		
	-> Keeping in mind to build the 32bit/64bit version consistent with the library you used. 
		For example, if you are compiling a 32bit version of ParaView, then use 32bit version 
		of OCC library and also 32bit Qt library. Otherwise errors in linking libraries. 


****Step 3. Run ParaView with this plugin

-> ParaView needs OCC and Qt runtime library for loading the plugin. There are
two approaches, one is setting correct environment variable for finding these
libraries, Or another way is copy all the necessary libraries into the folder of
ParaView executable. 

    -> 1st way: Right-click "Computer", then Properties->Anvanced system settings-
    >Environment variables, find the "Path" in the system variable and add for
    example ";$SALOME\PRODUCTS\OCCT-*.*.*\Win32\bin;$SALOME\PRODUCTS\qt-
    *.*.*\bin;$SALOME\PRODUCTS\tbb\bin;;$SALOME\PRODUCTS\tbb\bin\irml" (Change
    the $SALOME and "*" to the corresponding text). This requires administrator
    right.
    
    ->2nd way: Copy all the dependent libraries to the folder $PVBUILD\bin\Debug(or
    Release). These dependent libraries can be found in the above folders which
    asked to added into the "Path" variable.  These dependent libraries are :
    
    OCCReader.dll Qt3Support4.dll QtCLucene4.dll QtCore4.dll QtDeclarative4.dll
    QtDesigner4.dll QtDesignerComponents4.dll QtGui4.dll QtHelp4.dll
    QtMultimedia4.dll QtNetwork4.dll QtOpenGL4.dll QtScript4.dll
    QtScriptTools4.dll QtSql4.dll QtSvg4.dll QtTest4.dll QtWebKit4.dll
    QtXml4.dll QtXmlPatterns4.dll TKBO.dll TKBRep.dll TKBool.dll TKG2d.dll
    TKG3d.dll TKGeomAlgo.dll TKGeomBase.dll TKIGES.dll TKMath.dll TKMesh.dll
    TKOffset.dll TKPrim.dll TKSTEP.dll TKSTEP209.dll TKSTEPAttr.dll
    TKSTEPBase.dll TKShHealing.dll TKTopAlgo.dll TKXSBase.dll TKernel.dll
    irml.dll msvcp100.dll(optional) msvcr100.dll(Optional) tbb.dll
    tbbmalloc.dll 
    
-> After the above step, open paraview.exe, You need to load this library
manually for the first time. In ParaView menu Tools->Manage Plugins, click "Load
New" to load the "OCCReader.dll" in the some folder as paraview.exe , and select
"Auto Load" to load it automatically.

	
********************  How to use this plugin ***********************************
********************************************************************************

This plugin is used like other ParaView Reader plugin. In ParaView, click "Open" 
button, you can find "OpenCASCADE CAD file(*.step,*.stp,*.igs,*.iges,*.brep)" in 
The draw-down list. If yes, you can import any CAD files in those formats. If not,
please make sure that the plugin is loaded correctly. 

-> Loading a CAD file, click "Apply" to process the model. 

-> Display mode is for setting the display effect:

	->mode 1: wireframe mode
	->mode 2: shading mode
	->mode 3: wireframe & shading

->Deflection is for adjusting the display precision. The smaller deflection means 
  smaller tolerance, thus higher precision. 

->After changing the parameters, click "Apply" to take effect. 



Have fun playing with it!   :)
