********************************************************************************
OCCReader ---
A  plugin for visualizing CAD file in STEP,IGES and BREP format in  ParaView.
********************************************************************************

   OCCReader is a ParaView plugin for importing CAD files in Open CASCADE (OCC)
   open-source format STEP, IGES and BREP and visualizing in ParaView. This
   plugin facetes CAD solids into VTK Polydata.  With this plugin, the CAD
   geometry can be visualized together with physical fields, so that the
   traditional way of using STL file to visualizing geometry can be avoided.
   Three dispaly modes are provided-- wireframe(mode 1), shading(mode 2) and
   wireframe+shading (mode 3). The faceting accuracy can by increase by
   adjusting the "Deflection" parameter. This document provides instrutions for
   compiling the plugin.

   This plugin is developed based on the open-source SALOME platform with
   reuesage of classes. The LGPL lincese should be obeyed, see detail License
   Agreement of this plugin.

   Compiling this plugin ONLY IF the provided pre-compiled binary libraries are
   not suitable for your platform. This compilation is not straight-forward. Pre-
   compiled binary libraries are provided in the Github named as
   "OCCReader_Binaries". For compilation, see README_Install for more information.
   
   For any questions related to the use of this software/library you may contact
   Ulrich Fischer(ulrich.fischer@kit.edu) or, for technical assistance, contact
   Yuefeng Qiu (Yuefeng.qiu@kit.edu).
   
   Have fun playing with it!   :)
   
   
