// Copyright (C) 2014-2015  KIT-INR/NK 
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//

#ifndef OCC2VTK_H
#define OCC2VTK_H

#if defined WIN32
#  if defined OCC2VTK_EXPORTS
#    define OCC2VTK_EXPORT __declspec( dllexport )
#  elif defined OCCReader_EXPORTS
#    define OCC2VTK_EXPORT __declspec( dllexport )
#  else
#    define OCC2VTK_EXPORT __declspec( dllimport )
#  endif
#else
#  define OCC2VTK_EXPORT
#endif

#endif // OCC2VTK_H
