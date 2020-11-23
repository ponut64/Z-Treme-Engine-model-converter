#
# Ponut64 GVP Render Path Binary Converter
# Based on XL2 ZTP Binary Converter for SGL
#
#
# This converter takes:
# OBJ files
# 8-bit indexed color TGA or 24-bit RGB TGA as texture data
# 24-bit RGB texture data will be converted to indexed color
# 24-bit RGB TGA as color table / palette for RGB to indexed color conversion
# Keyframes as OBJ files
#
# This converter produces:
# SGL-compatible binary files in the format of PDATA
# pntbl (point table) 
# pntbl of single-objects is always 32-bit, 3-component vertices. 12 bytes each.
# pntbl of animated objects stores keyframes as 16-bit. 6 bytes each.
#
# pltbl (polygon table)
# contains first the 12 byte normal, then four two byte entries listing the vertices from the pntbl.
#
# attbl (attribute table)
# wasteful struct that loosely matches SEGA Saturn VDP1 command table
#
# cNorm (compressed normals)
# Only produced for keyframes. A 1-byte index into an animation normal table, ANORM.h
# System is broadly the same as Quake 2's.
# 
# NOTE:
# This converter ***DOES NOT PRODUCE*** normals per vertice
# This converter ***DOES NOT PRODUCE*** RGB texture data in the binary
# This converter ***IS INCOMPATIBLE*** with goraud shading and ZTP loading routines.
