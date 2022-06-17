#include "../COMMON.H"

int USE_SGL = 1; //For now, default option?

void writeU8(ofstream * file, unsigned char data);
void writeUint16(ofstream * file, uint16_t data);
void writeSint16(ofstream * file, int16_t data);
void writeUint32(ofstream * file, uint32_t data);
void writeSint32(ofstream * file, int32_t data);


#define Max_Normals (162)
VECTOR normLUT[] =
{
    #include "anorms.h"
};

/*
Model data header

********************* IT WILL CRASH IF IT ISN'T!*********************
********************* IT WILL CRASH IF IT ISN'T!*********************
****IF YOU EVER MODIFY THIS, BE AWARE: IT MUST BE 4-BYTE ALIGNED.****
********************* IT WILL CRASH IF IT ISN'T!*********************
********************* IT WILL CRASH IF IT ISN'T!*********************

0-1 byte: # of meshes
2-3 byte: # of textures
4-7 byte: total size of PDATA + compressed verts [rather, offset to end of]
8-11 byte: total size of textures [rather, offset to end of]
12-13 byte: # of keyframes
14-15 byte: x_radius
16-17 byte: y_radius
18-19 byte: z_radius
20 byte: polygon # of the first portal 
21-23 byte: padding.
*/
void WRITE_MDATA(ofstream * file, animated_model_t * aModel)
{
    writeUint16(file, aModel->nbModels);
    writeUint16(file, aModel->nbTextures);

    uint32_t countSize = 0;
    countSize=0;
    for (uint32_t i = 0; i<aModel->nbModels; i++) {
            countSize += aModel->model[i].nbPolygon * 20 ; //Each polygon = 20 bytes [12 bytes normal, 8 bytes vertices ID]
            countSize += aModel->model[i].nbPoint * 12; //Each vertice = 4 bytes for each component x,y,z 
    }

        countSize += (aModel->nbFrames * aModel->model[0].nbPoint * 7); //Each compressed vertex is a sint16_t[3] + compressed normal, 1 byte
        writeUint32(file, countSize);


    for (uint32_t i = 0; i<aModel->nbTextures; i++) {
        countSize += (aModel->texture[i].height * aModel->texture[i].width); //At this point, you would denote a size change for depth.
    }
    writeUint32(file, countSize);

    countSize=0;

    cout << "\n";
	cout << "X radius: " << aModel->x_radius << "\n";
	cout << "Y radius: " << aModel->y_radius << "\n";
	cout << "Z radius: " << aModel->z_radius << "\n";

    //Animation : Updated 2018/05/31
    writeUint16(file, aModel->nbFrames);
	//Radius : 2020
    writeUint16(file, aModel->x_radius);
    writeUint16(file, aModel->y_radius);
    writeUint16(file, aModel->z_radius);
	//Portal stuff: 2021
	unsigned char first_portal_written_to_header = 255;
        //ATTR, 12 bytes each // 5 bytes, in my case
        for (unsigned int ii=0; ii< aModel->model[0].nbPolygon; ii++)
        {
			if(aModel->texture[aModel->model[0].pltbl[ii].texture].GV_ATTR.portal_information == 254)
			{
				first_portal_written_to_header = ii;
			}
        }
	writeUint32(file, (unsigned int)first_portal_written_to_header);
	
	if(first_portal_written_to_header == 255)
	{
	cout << "\n Mesh had no portals. ";
	} else {
	cout << "\n First portal: " << (unsigned int)first_portal_written_to_header << " \n"; 
	}
	
}

void	crossf(float vector1[3], float vector2[3], float output[3])
{
	output[0] = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
	output[1] = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
	output[2] = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
}

void	fnormalize(float vec[3])
{
	float scalar = 1.0/sqrtf((vec[0] * vec[0]) + (vec[1] * vec[1]) + (vec[2] * vec[2]));
	vec[0] = vec[0] * scalar;
	vec[1] = vec[1] * scalar;
	vec[2] = vec[2] * scalar;
}

float	fdot(float vec1[3], float vec2[3])
{
	return (vec1[0] * vec2[0]) + (vec1[1] * vec2[1]) + (vec1[2] * vec2[2]);
}

	void	rotate_by_rule(float verts[4][3], short * vIDs, int component, bool reverse)
	{
		float tempVert[3] = {0, 0, 0};
		short tempID = 0;
		int flipcnt = 0;
		
			if(reverse != true)
				{
		while(verts[0][component] < 0)
			{
				tempVert[0]	= verts[0][0];
				tempVert[1]	= verts[0][1];
				tempVert[2]	= verts[0][2];
				
				verts[0][0]	= verts[1][0];
				verts[0][1]	= verts[1][1];
				verts[0][2]	= verts[1][2];
				
				verts[1][0]	= verts[2][0];
				verts[1][1]	= verts[2][1];
				verts[1][2]	= verts[2][2];
				
				verts[2][0]	= verts[3][0];
				verts[2][1]	= verts[3][1];
				verts[2][2]	= verts[3][2];
				
				verts[3][0]	= tempVert[0];
				verts[3][1]	= tempVert[1];
				verts[3][2]	= tempVert[2];
				
				tempID	= vIDs[0];
				vIDs[0]	= vIDs[1];
				vIDs[1]	= vIDs[2];
				vIDs[2]	= vIDs[3];
				vIDs[3]	= tempID;
				
				flipcnt++;
			}
				} else {
		while(verts[0][component] > 0)
			{
				tempVert[0]	= verts[0][0];
				tempVert[1]	= verts[0][1];
				tempVert[2]	= verts[0][2];
				
				verts[0][0]	= verts[1][0];
				verts[0][1]	= verts[1][1];
				verts[0][2]	= verts[1][2];
				
				verts[1][0]	= verts[2][0];
				verts[1][1]	= verts[2][1];
				verts[1][2]	= verts[2][2];
				
				verts[2][0]	= verts[3][0];
				verts[2][1]	= verts[3][1];
				verts[2][2]	= verts[3][2];
				
				verts[3][0]	= tempVert[0];
				verts[3][1]	= tempVert[1];
				verts[3][2]	= tempVert[2];
				
				tempID	= vIDs[0];
				vIDs[0]	= vIDs[1];
				vIDs[1]	= vIDs[2];
				vIDs[2]	= vIDs[3];
				vIDs[3]	= tempID;
				
				flipcnt++;
			}
				}
	}
	
	void	permutate(float verts[4][3], short * vIDs) //counter-clockwise
	{
	float tempVert[3] = {0, 0, 0};
	short tempID = 0;
				tempVert[0]	= verts[0][0];
				tempVert[1]	= verts[0][1];
				tempVert[2]	= verts[0][2];
				
				verts[0][0]	= verts[1][0];
				verts[0][1]	= verts[1][1];
				verts[0][2]	= verts[1][2];
				
				verts[1][0]	= verts[2][0];
				verts[1][1]	= verts[2][1];
				verts[1][2]	= verts[2][2];
				
				verts[2][0]	= verts[3][0];
				verts[2][1]	= verts[3][1];
				verts[2][2]	= verts[3][2];
				
				verts[3][0]	= tempVert[0];
				verts[3][1]	= tempVert[1];
				verts[3][2]	= tempVert[2];
				
				tempID	= vIDs[0];
				vIDs[0]	= vIDs[1];
				vIDs[1]	= vIDs[2];
				vIDs[2]	= vIDs[3];
				vIDs[3]	= tempID;
	}
	

void	poly_face_sort(polygon_t * poly, vertex_t * vList)
{
	short vIDs[4] = {0, 0, 0, 0};
	float verts[4][3];
	float center[3] = {0, 0, 0};
		
		//Load vertice IDs
		vIDs[0] = poly->vertIdx[0];
		vIDs[1] = poly->vertIdx[1];
		vIDs[2] = poly->vertIdx[2];
		vIDs[3] = poly->vertIdx[3];
		
		//Load vertices
		for(int l = 0; l < 4; l++)
		{
		verts[l][X] = vList[vIDs[l]].point[X];
		verts[l][Y] = vList[vIDs[l]].point[Y];
		verts[l][Z] = vList[vIDs[l]].point[Z];
		//Add up center
		center[X] += verts[l][X];
		center[Y] += verts[l][Y];
		center[Z] += verts[l][Z];
		}
		//Get center
		center[X] = center[X] / 4.0;
		center[Y] = center[Y] / 4.0;
		center[Z] = center[Z] / 4.0;	
		//Center vertices to themselves
		for(int h = 0; h < 4; h++)
		{
		verts[h][X] -= center[X];
		verts[h][Y] -= center[Y];
		verts[h][Z] -= center[Z];
		fnormalize(verts[h]);
		}
		
		/*
		OPERATING THEORY
		
		You're putting into this loop soup for a polygon.
		It is a properly facing order of vertices, in terms of its normal and facing towards the near plane,
		but the direction the texture will draw is random.
		
		You want to force the textures to all draw with a particular visual orientation.
		To accomplish this, you first have to re-order the soup so vertice 0 of the polygon is in a known visual location.
		We will target "on the left" as the orientation of vertice 0.
		
		In theoretical terms, this is based on a screenspace transformation of the vertices in a given orientation.
		Our orientation screenspace Y being as close to the texture's Y axis as we can get it.
		But we will use a shortcut!
		
		The polygon's normal can be used to find out which axis the polygon most faces.
		The axis the polygon most faces is the axis we want to align it to.
		We also want to bias two axis. If we equally treat all three,
		the dominant axis (Y) will interfere with the alignment of the others on steep slopes.
		
		Take this alignment, for example:
		
				y-
		
			z+		1
			 0		
		x+		O		x-
					2
				3	z-
		
				y+
		
		That's probably not a good graphic, but 0 and 3 are supposed to be equially distant from origin in Z,
		but also have a large gap in Y and X. 1 and 2 don't vary much in X, but vary a lot in Y and Z.
		It's an ambiguous shape: It could align Y, or X.
		We would prefer X unless it is supremely dominant in Y to the point where no other axis is useful.
		
		The shortcut, then, is that the dominant axis of a normal is the axis in which is decimated when forcing the orientation.
		If it is dominant Z, the Z value of the vertices is not useful. 
		However, we do need to know if it is dominant in -Z or +Z.
		This is because looking at +Z puts X+ on the left. Looking at -Z puts X+ on the right.
		When we want to put vertice 0 "on the left", this is important.
		So if the normal's dominant axis is + or -, that determines if we want a positive second axis or a negative second axis.
		
		The sorting axis is never Y because Y is the overall dominant axis.
		The Y bias will be the same between X+ and Z+. When you let the dominant axis dominate the sorting,
		you get sorting relative to views from the dominant axis, which is ambiguous when viewing from X or Z projections.
		
		However, there are edge-cases where the dominant axis must be consulted wherein the shape of the polygon
		is ambiguous due to overlaps of vertice 1, 3, or 0 on the Y axis.
		
		When we know where vertice 0 is, we can either leave that as the orientation..
		or permutate the polygon a couple more times to get the desired orientation.
		
		*/
		
			if(fabs(poly->normal[Y]) > 0.97 && poly->normal[Y] > 0)
			{

				if(verts[0][X] < 0)
				{
					//
					rotate_by_rule(verts, vIDs, 0, false);
					//Pre-mutation to decided alignment 
					permutate(verts, vIDs);
				} else if(verts[0][X] > 0)
				{
					//
					rotate_by_rule(verts, vIDs, 0, true);
					//Pre-mutation to decided alignment 
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					permutate(verts, vIDs);
				}
				
				
			} else if(fabs(poly->normal[Y]) > 0.97 && poly->normal[Y] < 0){

				if(verts[0][X] < 0)
				{
					//
					rotate_by_rule(verts, vIDs, 0, false);
					//Pre-mutation to decided alignment 
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					permutate(verts, vIDs);
				} else if(verts[0][X] > 0)
				{
					//
					rotate_by_rule(verts, vIDs, 0, true);
					//Pre-mutation to decided alignment 
					permutate(verts, vIDs);
				}
				
		} else if(fabs(poly->normal[X]) >= fabs(poly->normal[Z]) && fabs(poly->normal[Y]) < fabs(poly->normal[X]))
		{
			if(poly->normal[X] >= 0)
			{

				if(verts[0][Z] < 0)
				{
					//
					rotate_by_rule(verts, vIDs, 2, false);
					//Pre-mutation to decided alignment 
					permutate(verts, vIDs);
				} else if(verts[0][Z] > 0)
				{
					//
					rotate_by_rule(verts, vIDs, 2, true);
					//Pre-mutation to decided alignment 
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					permutate(verts, vIDs);
				}
				
				////////////////////////////////////////////
				//Post-mutation of ambiguous polygonm shapes
				////////////////////////////////////////////
				if(verts[2][Y] >= verts[0][Y])
				{
					//
					permutate(verts, vIDs);
				} else if(verts[2][Y] >= verts[1][Y])
				{
					//
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					permutate(verts, vIDs);
				} else if(verts[1][Y] < verts[3][Y] && verts[1][Y] < verts[0][Y])
				{
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					//
				}
				////////////////////////////////////////////
				/*
				so far..	1y < 03y
							2y < 01y
						y-
					   2			
					   1
				z-		 03		z+
						y+
				3 = 0 or 3 ~ 0 or 3 != 0 or 3 !~ 0, it doesn't matter.
				This particular odd solution is documented because it's not intuitive.
				*/
			
			} else {
				if(verts[0][Z] < 0)
				{
					//
					rotate_by_rule(verts, vIDs, 2, false);
					//Pre-mutation to decided alignment 
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					permutate(verts, vIDs);
				} else if(verts[0][Z] > 0)
				{
					//
					rotate_by_rule(verts, vIDs, 2, true);
					//Pre-mutation to decided alignment 
					permutate(verts, vIDs);
				}
				
				////////////////////////////////////////////
				//Post-mutation of ambiguous polygonm shapes
				////////////////////////////////////////////
				if(verts[2][Y] > verts[0][Y])
				{
					//
					permutate(verts, vIDs);
				} else if(verts[2][Y] > verts[1][Y])
				{
					//
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					permutate(verts, vIDs);
				} else if(verts[1][Y] < verts[3][Y] && verts[1][Y] < verts[0][Y])
				{
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					//
				}
				////////////////////////////////////////////
	
			}
		} else {
			if(poly->normal[Z] >= 0)
			{
					
				if(verts[0][X] < 0)
				{
					//
					rotate_by_rule(verts, vIDs, 0, false);
					//Pre-mutation to decided alignment 
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					permutate(verts, vIDs);
				} else if(verts[0][X] > 0)
				{
					//
					rotate_by_rule(verts, vIDs, 0, true);
					//Pre-mutation to decided alignment 
					permutate(verts, vIDs);
				}
				
				////////////////////////////////////////////
				//Post-mutation of ambiguous polygonm shapes
				////////////////////////////////////////////
				if(verts[2][Y] >= verts[0][Y])
				{
					permutate(verts, vIDs);
					//
				} else if(verts[2][Y] >= verts[1][Y])
				{
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					//
				} else if(verts[1][Y] < verts[3][Y] && verts[1][Y] < verts[0][Y])
				{
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					//
				}
				////////////////////////////////////////////
					
			} else {
					
				if(verts[0][X] < 0)
				{
					//
					rotate_by_rule(verts, vIDs, 0, false);
					//Pre-mutation to decided alignment 
					permutate(verts, vIDs);
				} else if(verts[0][X] > 0)
				{
					//
					rotate_by_rule(verts, vIDs, 0, true);
					//Pre-mutation to decided alignment 
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					permutate(verts, vIDs);
				}
				
				////////////////////////////////////////////
				//Post-mutation of ambiguous polygonm shapes
				////////////////////////////////////////////
				if(verts[2][Y] >= verts[0][Y])
				{
					permutate(verts, vIDs);
					//
				} else if(verts[2][Y] >= verts[1][Y])
				{
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					//
				} else if(verts[1][Y] < verts[3][Y] && verts[1][Y] < verts[0][Y])
				{
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					permutate(verts, vIDs);
					//
				}
				////////////////////////////////////////////

			}
		} 
		
		//Cleanup
		center[X] = 0;
		center[Y] = 0;
		center[Z] = 0;
		//Final alignment?
		//This is because this stuff was demo'd on Saturn with unflipped flipped textures.
		//Remember, TGAs start with the origin at the bottom left.
		//In this program, we unflip the textures (so they are read in as top-left origin).
		permutate(verts, vIDs);
		permutate(verts, vIDs);
		
		////////////////////////////////////////////
		//	Triangle Handling
		//	Rather than help with the texture directions, triangles are always oriented such that vertex 2 == vertex 3.
		//	At this step FLIP directives could be written to the attributes of the polygon to preserve direction in some cases.
		////////////////////////////////////////////
			if(vIDs[0] == vIDs[1] || vIDs[0] == vIDs[3] || vIDs[1] == vIDs[2])
			{
				do{
					permutate(verts, vIDs);
				}while(vIDs[2] != vIDs[3]);
			}
		
		//Write vertice IDs
			poly->vertIdx[0] = vIDs[0];
			poly->vertIdx[1] = vIDs[1];
			poly->vertIdx[2] = vIDs[2];
			poly->vertIdx[3] = vIDs[3];
			

		
		
		
}

/*****
FOR SGL (mainly for you Ponut64!) : This writes all the PDATA in a sequential order
*****/
void WRITE_SGL_PDATA(ofstream * file, animated_model_t * aModel)
{
	
	unsigned char first_portal = 254;
	
    for (unsigned int i=0; i<aModel->nbModels; i++){
        //PDATA, including buffers for the pointers
        writeUint32(file, 0); //Empty 4-byte area for the PNTBL pointer [verts]
        writeUint32(file, aModel->model[i].nbPoint); //# of point
        writeUint32(file, 0); //Empty 4-byte area for the PLTBL pointer [polys]
        writeUint32(file, aModel->model[i].nbPolygon); //# of poly
		writeUint32(file, 0); //Empty 4-byte area for the ATTBL pointer [attributes]
       //Written type is PDATA, there are no per-vertice normals.

        //POINT, 12 bytes each
        for (unsigned int ii=0; ii<aModel->model[i].nbPoint; ii++) {
            for (unsigned int j=0; j<3; j++) {
                writeSint32(file,toFIXED(aModel->model[i].pntbl[ii].point[j]));
            }
        }
		
        //POLYGON, 12 bytes for normals and 8 bytes for vertices
        for (unsigned int ii=0; ii< aModel->model[i].nbPolygon; ii++) {
		
			//spesh[ii] = 0;
			
			poly_face_sort(&aModel->model[i].pltbl[ii], &aModel->model[i].pntbl[0]);

            //Normals
            for (unsigned int j=0; j<3; j++) {
                writeSint32(file, toFIXED(aModel->model[i].pltbl[ii].normal[j]));
            }
            //Vertices //Vertice index, e.g. a polygon points to vertices 16, 21, 17, 20.
            for (unsigned int j=0; j<4; j++) {
                writeUint16(file, aModel->model[i].pltbl[ii].vertIdx[j]);
            }
        }
        //ATTR, 12 bytes each // 5 bytes, in my case
        for (unsigned int ii=0; ii< aModel->model[i].nbPolygon; ii++)
        {
            // file->write((char*)&aModel->texture[aModel->model[i].pltbl[ii].texture].SGL_ATTR.flag, sizeof(uint8_t));
            // file->write((char*)&aModel->texture[aModel->model[i].pltbl[ii].texture].SGL_ATTR.sorting, sizeof(uint8_t));
            // writeUint16(file, aModel->texture[aModel->model[i].pltbl[ii].texture].SGL_ATTR.texno);
            // writeUint16(file, aModel->texture[aModel->model[i].pltbl[ii].texture].SGL_ATTR.atrb);
            // writeUint16(file, aModel->texture[aModel->model[i].pltbl[ii].texture].SGL_ATTR.colno);
            // writeUint16(file, aModel->texture[aModel->model[i].pltbl[ii].texture].SGL_ATTR.gstb);
            // writeUint16(file, aModel->texture[aModel->model[i].pltbl[ii].texture].SGL_ATTR.dir);

			file->write((char*)&aModel->texture[aModel->model[i].pltbl[ii].texture].GV_ATTR.render_data_flags, sizeof(uint8_t));
			if(aModel->texture[aModel->model[i].pltbl[ii].texture].GV_ATTR.portal_information == 254)
			{
			file->write((char*)&first_portal, sizeof(uint8_t));
			first_portal = ii;
			} else {
			file->write((char*)&aModel->texture[aModel->model[i].pltbl[ii].texture].GV_ATTR.portal_information, sizeof(uint8_t));
			}
			file->write((char*)&aModel->texture[aModel->model[i].pltbl[ii].texture].GV_ATTR.first_sector_number, sizeof(uint8_t));
			file->write((char*)&aModel->texture[aModel->model[i].pltbl[ii].texture].GV_ATTR.second_sector_number, sizeof(uint8_t));
			writeUint16(file, aModel->texture[aModel->model[i].pltbl[ii].texture].GV_ATTR.texno);
        }

    }

}

void WRITE_TEXTURES(ofstream * file, animated_model_t * aModel)
{
    cout << "Writing the textures to binary file...\n\n";
	int tSize;
    texture_t * t;
    for (unsigned short i=0; i<aModel->nbTextures; i++)
    {
			t=&aModel->texture[i];
			writeUint16(file, 0x7F7F);
			writeU8(file, t->height);
			writeU8(file, t->width);
			tSize = t->height * t->width;
			for(int k = 0; k < tSize; k++)
			{
				writeU8(file, t->pixel[k].a);
			}
    }

    cout << "Writing the textures to text file (for debugging)...\n\n";
    ofstream tFile("OUT/TEXTURES.TXT", ios::out);
    if (!tFile.is_open()) {cout << "ERROR WRITING TEXT FILE TEXTURE DATA...\n"; return;}

}


uint8_t normalLookUp(float * curNormal)
{
    uint32_t  bestRes = 0;
    float   bestDist = 65535.0;

    for (uint32_t i=0; i<Max_Normals; i++)
    {
        float dist = sqrtf(pow(curNormal[X]-normLUT[i][X], 2) + pow(curNormal[Y]-normLUT[i][Y], 2) + pow(curNormal[Z]-normLUT[i][Z], 2));
        if ((dist) < (bestDist))
        {
            bestDist=dist;
            bestRes=i;
        }
    }
    return bestRes;
}

extern void recalculateNormals(model_t *);



/**
Writes the vertex data and compressed normals (to keep SGL happy)
**/
void writeAnim(ofstream * file, animated_model_t * aModel, unsigned int FrameID)
{
    int32_t bSint32 = swap_endian_sint(FrameID);
    uint8_t bUint8 = 0;
    file->write((char*)&bSint32, sizeof(int32_t));
    file->write((char*)&bSint32, sizeof(int32_t));
    unsigned int tot_vertices=0, tot_normals=0;

/********************************
WRITES COMPRESSED VERTICES AS 8.8 FIXED-POINT FORMAT (2-bytes)
[There's nothing special about these except the way they are written]
*********************************/
    for (unsigned int i=0; i< aModel->nbModels; i++ ) {
        for (unsigned int ii=0; ii<(aModel->model[i].nbPoint); ii++) {
            for (unsigned int j=0; j<3; j++) {
                writeSint16(file, toFIXED8(aModel->keyFrames[FrameID].cVert[tot_vertices].point[j]));
            }
            tot_vertices++;
        }
    }
    while (tot_vertices%2 !=0) {
        writeSint16(file, 0);
        tot_vertices++;
    }
//

/********************************
WRITES COMPRESSED NORMALS AS 1-BYTE ANORM.H ENTRY
*********************************/
    for (unsigned int i=0; i< aModel->nbModels; i++ ) {
        for (unsigned int ii=0; ii<(aModel->model[i].nbPolygon); ii++) {
            bUint8 = (uint8_t)normalLookUp((float*)&aModel->model[i].pltbl[ii].normal[X]); //baseModel->animation[FrameID].cNorm[tot_normals]=(uint8_t)(normalLookUp(kfModel->pol[i].pltbl[ii].norm));
            file->write((char*)&bUint8, sizeof(uint8_t));
            tot_normals++;
        }
    }
    while (tot_normals%4 !=0) {
        file->write((char*)(&bSint32), sizeof(int8_t));
        tot_normals++;
    }
}

	////////////////////////////////////////////
	// Ponut64 Addition
	// Write item data
	// 0byte : total # of items
	// 1byte : number of unique items
	// then : 8 bytes per item : #, x, y, z, each a <short>
	////////////////////////////////////////////
void	write_item_data(ofstream * file, animated_model_t * aModel)
{
	writeU8(file, (unsigned char)number_of_items);
	writeU8(file, (unsigned char)number_of_unique_items);
	// If there are no items, don't write any more.
	if(number_of_items == 0)
	{
		return;
	}
	// Otherwise, write:
	// 1. Item number, as an unsigned short
	// 2. Item position (as offets from center of mesh), in: X,Y,Z signed short (**thusly only whole integers)
	// That makes item data as:
	// number, x pos, y pos, z pos : 2 bytes * 4 = 8 bytes for each item
	for(int i = 0; i < number_of_items; i++)
	{
		writeUint16(file, (unsigned short)items[i]);
		writeSint16(file, (short)((int)item_positions[i][X]));
		writeSint16(file, (short)((int)item_positions[i][Y]));
		writeSint16(file, (short)((int)item_positions[i][Z]));
	}
}


/********************************
* WIP : Writes data to custom binary format

WRITTEN ORDER:
 **Ponut64 Modified**
FIRST: HEADER
SECOND: PDATA
THIRD: ANIMATION DATA, IF PRESENT
FOURTH: TEXTURES
FINALLY: ITEM DATA, IF PRESENT
*********************************/
void write_model_binary(ofstream * file, animated_model_t * aModel)
{

    WRITE_MDATA(file, aModel);
	WRITE_SGL_PDATA(file, aModel);

	//Making duplicate base data array of the model's vertices data
    vertex_t * v[aModel->nbModels];
    for (unsigned int i=0; i<aModel->nbModels; i++)
	{
        v[i] = aModel->model[i].pntbl;
	}
	//Writing Animation data
    for (unsigned int i=0; i<aModel->nbFrames; i++)
    {
        uint32_t v_count = 0;
        for (unsigned int ii=0; ii<aModel->nbModels; ii++)
        {
            aModel->model[ii].pntbl = &aModel->keyFrames[i].cVert[v_count];
            v_count += aModel->model[ii].nbPoint;
            recalculateNormals(&aModel->model[ii]);
        }

        writeAnim(file, aModel, i);
        for (unsigned int i=0; i<aModel->nbModels; i++)
        aModel->model[i].pntbl = v[i];
    }
	
    WRITE_TEXTURES(file, aModel);
	
	////////////////////////////////////////////
	// Ponut64 Addition
	// Write item data
	////////////////////////////////////////////
	write_item_data(file, aModel);
}


