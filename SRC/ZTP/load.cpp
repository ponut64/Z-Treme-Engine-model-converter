#include "../COMMON.H"

    extern int ReadTGAFile (string folder, texture_t * texture);
	extern int ReadPaletteFile (void);
	
	int		item_exception_vertices[64][4];
	int		items[64];
	int		item_number_package[64];
	float	item_positions[64][XYZ];
	int		number_of_items = 0;
	int		number_of_unique_items = 0;

int generate_texture_list(string inString, animated_model_t * model)
{
    if (inString=="None") return -1;
    for (unsigned int i=0; i<model->nbTextures; i++)
    {
        if (inString == model->texture[i].name) return i;
    }
    model->texture[model->nbTextures].name = inString;
    model->texture[model->nbTextures].textureId = model->nbTextures;
    model->nbTextures++;
    return model->nbTextures-1;
}

/**Old implementation, weird code, it works, so who cares?**/
int sort_faces(string inString, model_t * t_PDATA, polygon_t * bufPol, int index, int total_verts)
{
            string f1, f2, f3;
            char bufferChar[128]={}; //Don't ask, I'm more used to C than C++, so just went "fuck it"....
            int i = 0;
            for (i=0; i<128; i++)
            {
				//Find the first '/', then break from this loop.
                if (inString[i]=='/') {i++; break;}
                bufferChar[i]=inString[i];
            }
            f1 = bufferChar;
            int charPtr=0;            char bufferChar2[128]={};            f2 = "";
            for (i=i; i<128; i++)
            {
				//Find the next '/' then break from this loop
                if (inString[i]=='/') {i++; break;}
                bufferChar2[charPtr]= inString[i];
                charPtr++;
            }
            f2 = bufferChar2;
            charPtr=0; char bufferChar3[128]={}; f3 = "";
            for (i=i; i<128; i++)
            {
				//Find the next '/' then break from this loop
                if (inString[i]=='/') {i++; break;}
                bufferChar3[charPtr]=inString[i];
                charPtr++;
            }
            f3=bufferChar3;

  int i1, i2;
  if (f1 != "")   i1 = std::stoi(f1) - 1;
  else (i1 = 0);
  if (f2 != "") i2 = std::stoi(f2) - 1;
  else (i2 = 0);

  bufPol->vertIdx[index]=i1-total_verts;

  return i2;
}

/****
*Main function to read the OBJ file
*Param : Obj file, your model data (_MOD_DATA) and boolean if it's a base model
*****/
bool load_OBJ_to_mesh(ifstream *file, animated_model_t * aModel, int keyFrameID)
{
    string line;
    int pointer = 0;
    int textpointer = -1;
    int bufTotalVerts = 0;
    int MeshPtr = -1;

	int working_with_item = 0;

    if (!file->is_open()) return false;
    file->seekg(pointer, ios::beg);

    model_t * o_PDATA;

	///////////////////////////////////////////
	// Ponut64 Addition
	// Zero-out model's radius on source model (keyframe -1)
	///////////////////////////////////////////
	if (keyFrameID==-1)
	{
		aModel->x_radius = 0;
		aModel->y_radius = 0;
		aModel->z_radius = 0;
	}

    while(!file->eof())
    {
        getline(*file, line);
        //cout << line << "\n";
        if (((line[0] == 'o' && line[1] == ' ') || (line[0]== 'g')) && (keyFrameID==-1))
        {
            if (MeshPtr >= 0) bufTotalVerts += aModel->model[MeshPtr].nbPoint;
            MeshPtr++;
            aModel->model[MeshPtr].nbPoint=0;
            aModel->model[MeshPtr].nbPolygon=0;
            aModel->model[MeshPtr].pntbl = new vertex_t[1024];
            aModel->model[MeshPtr].pltbl = new polygon_t[1024];

            o_PDATA = &aModel->model[MeshPtr];
            stringstream NameSS(line);            string buffer;            getline(NameSS, buffer, ' ');            NameSS >> buffer;
            cout << "\nMESH NAME : " << buffer << "\n";
            aModel->name = buffer;
        }
        else if (line[0] == 'v' && line[1] == ' ')
        {
            stringstream PointSS(line);
            string uselessbuffer;
            getline(PointSS, uselessbuffer, ' ');
            string sx = ""; string sy = ""; string sz = "";
            PointSS >> sx >> sy >> sz;

            float fx=std::atof(sx.c_str()); float fy=std::atof(sy.c_str()); float fz=std::atof(sz.c_str());

            if (keyFrameID==-1)
            {
                o_PDATA->pntbl[o_PDATA->nbPoint].point[X] = fx;
                o_PDATA->pntbl[o_PDATA->nbPoint].point[Y] = fy;
                o_PDATA->pntbl[o_PDATA->nbPoint].point[Z] = fz;
                o_PDATA->nbPoint++;
				
				///////////////////////////////////////////
				// Ponut64 Addition
				// Check if the point is larger than the previously largest number on any axis.
				// If it is, write it as the radius.
				///////////////////////////////////////////
				aModel->x_radius = ((unsigned short)(fabs(fx)) > aModel->x_radius) ? fabs(fx) : aModel->x_radius;
				aModel->y_radius = ((unsigned short)(fabs(fy)) > aModel->y_radius) ? fabs(fy) : aModel->y_radius;
				aModel->z_radius = ((unsigned short)(fabs(fz)) > aModel->z_radius) ? fabs(fz) : aModel->z_radius;
            }
            else
            {
                aModel->keyFrames[keyFrameID].cVert[bufTotalVerts].point[X] = fx;
                aModel->keyFrames[keyFrameID].cVert[bufTotalVerts].point[Y] = fy;
                aModel->keyFrames[keyFrameID].cVert[bufTotalVerts].point[Z] = fz;
                bufTotalVerts++;
            }

        }
        else if (line[0]== 'v' && line[1]== 't') //Texture coordinates...not implemented
        {
            /*stringstream TCoord(line);         string bufferStr;
            getline(TCoord, bufferStr, ' ');
            string tc[2] = {""};
            TCoord >> tc[0] >> tc[1];
            texture_coord[tot_tex_coord][0] = std::stof(tc[0]);
            texture_coord[tot_tex_coord][1] = std::stof(tc[1]);
            tot_tex_coord++;*/
        }
        else if ((line[0]=='u' && line[1]=='s' && line[2]=='e' && line[3]=='m' && line[4]=='t' && line[5]=='l')&& (keyFrameID==-1)) //Yeah, I know we all got it after the 2 letters, but just in case some weird Obj format popup...
        {
			//cout << aModel->nbTextures << "\n";
            stringstream MtlSS(line);         string bufferStr;
            getline(MtlSS, bufferStr, ' ');
            string mtlName = {""};
            MtlSS >> mtlName;
			std::size_t findItem = mtlName.find("ITEM_");
			if(findItem != 0)
			{
			// Not an item data material. Process as a texture.
            textpointer=generate_texture_list(mtlName, aModel);
			working_with_item = 0;
			} else if(findItem == 0)
			{
			//////////////////////////////////////////////////////////////////////
			//	Ponut64 Addition
			//	This code is very spaghetti and somewhat challenging to crawl through...
			//
			//	This section checks if the material (in the object file) has a prefix of "ITEM_".
			//	Note the OBJ format: It is plaintext, and a material definition precedes all polygons of that material.
			//	If the prefix "ITEM_" is found, the text following the prefix is converted to integer (stoi).
			//	This information is written to the item number package array, which lists the unique item numbers.
			//	This material/texture is not stored in the texture list.
			//////////////////////////////////////////////////////////////////////
				mtlName.erase(mtlName.begin(), mtlName.begin()+5);
				//Convert string to integer (stoi).
				item_number_package[number_of_unique_items] = stoi(mtlName, 0);
				number_of_unique_items++;
				//We're working with an item now.
				working_with_item = 1;
			}
		}
        else if ((line[0] == 'f' && line[1] == ' ')&& (keyFrameID==-1))
        {
            stringstream FaceSS(line);
            string bufferStr;
            getline(FaceSS, bufferStr, ' ');
            string p[4];
            FaceSS >> p[0] >> p[1] >> p[2] >> p[3];
            if (p[3]=="") p[3]=p[2];

            polygon_t bufPol;
            sort_faces(p[3], o_PDATA, &bufPol, 0, bufTotalVerts);
            sort_faces(p[2], o_PDATA, &bufPol, 1, bufTotalVerts);
            sort_faces(p[1], o_PDATA, &bufPol, 2, bufTotalVerts);
            sort_faces(p[0], o_PDATA, &bufPol, 3, bufTotalVerts);

			//////////////////////////////////////////////////////////////////////
			//	Ponut64 Addition
			//	This code is very spaghetti and somewhat challenging to crawl through...
			//	I'm not making it any better.
			//
			//	Note the OBJ format: It is plaintext, and a material definition precedes all polygons of that material.
			//	If the polygon in the OBJ followed an "ITEM_" material, we should have the item # stored.
			//	Convert the polygon information into a single-point by averaging the position of all four points.
			//	Then store the information from the item name as associated with this item's location.
			//////////////////////////////////////////////////////////////////////
			if(working_with_item == 1)
			{
				unsigned int ptv[4] = {
					bufPol.vertIdx[0],
					bufPol.vertIdx[1],
					bufPol.vertIdx[2],
					bufPol.vertIdx[3]
				};
				item_positions[number_of_items][X] =  (o_PDATA->pntbl[ptv[0]].point[X] +
														o_PDATA->pntbl[ptv[1]].point[X] +
														o_PDATA->pntbl[ptv[2]].point[X] +
														o_PDATA->pntbl[ptv[3]].point[X]
														) / 4.0;
				item_positions[number_of_items][Y] =  (o_PDATA->pntbl[ptv[0]].point[Y] +
														o_PDATA->pntbl[ptv[1]].point[Y] +
														o_PDATA->pntbl[ptv[2]].point[Y] +
														o_PDATA->pntbl[ptv[3]].point[Y]
														) / 4.0;
				item_positions[number_of_items][Z] =  (o_PDATA->pntbl[ptv[0]].point[Z] +
														o_PDATA->pntbl[ptv[1]].point[Z] +
														o_PDATA->pntbl[ptv[2]].point[Z] +
														o_PDATA->pntbl[ptv[3]].point[Z]
														) / 4.0;
														
				cout << "Created Item at Pos: \n";
				cout << item_positions[number_of_items][X] << "\n ";
				cout << item_positions[number_of_items][Y] << "\n ";
				cout << item_positions[number_of_items][Z] << "\n ";
														
				item_exception_vertices[number_of_items][0] = ptv[0];
				item_exception_vertices[number_of_items][1] = ptv[1];
				item_exception_vertices[number_of_items][2] = ptv[2];
				item_exception_vertices[number_of_items][3] = ptv[3];
				items[number_of_items] = item_number_package[number_of_unique_items-1];
				number_of_items++;
			} else {
				o_PDATA->pltbl[o_PDATA->nbPolygon]=bufPol;
				o_PDATA->pltbl[o_PDATA->nbPolygon].texture = textpointer;
				o_PDATA->nbPolygon++;
			}
        }
        pointer++;
    }

    file->clear();

    if (keyFrameID==-1)
    {
        extern void recalculateNormals(model_t * pol);
        extern void pointNormals(model_t * pol);
        aModel->nbModels=MeshPtr + 1;
        for (unsigned i = 0; i<aModel->nbModels; i++)
        {
            recalculateNormals(&aModel->model[i]);
            pointNormals(&aModel->model[i]);
        }
    }

    return true;
}


void specialConditions(unsigned short startPtr, unsigned short endPtr, animated_model_t * aModel)
{
    if (aModel->nbTextures <= 0) return;

    texture_t * t;
	
	int numDual = 0;
	int numMesh = 0;
	int numMedu = 0;
	int numNormal = 0;
	
    for (unsigned short i=startPtr; i<endPtr; i++)
    {
        t = &aModel->texture[i];
	//////////////////////////////////////////////
	// Ponut64 Addition
	// Find the texture types based on the prefix of the name and generate the appropriate attbl for it.
	//////////////////////////////////////////////
        std::size_t findDual = t->name.find("DUAL_");	//Dual-planes
        std::size_t findMesh = t->name.find("MESH_");	//Mesh polys
        std::size_t findMedu = t->name.find("MEDU_");	//Mesh + dual plane polys
	
		if(findDual == 0) //If DUAL is found at the start
		{
			numDual++;
		t->SGL_ATTR=ATTRIBUTE(Dual_Plane,SORT_CEN, i, 0, No_Gouraud,
		Window_In|MESHoff|HSSon|ECdis|SPdis|CL64Bnk,sprNoflip, UseNearClip);
		} else if(findMesh == 0)
		{
			numMesh++;
		t->SGL_ATTR=ATTRIBUTE(Single_Plane,SORT_CEN, i, 0, No_Gouraud,
		Window_In|MESHon|HSSon|ECdis|SPdis|CL64Bnk,sprNoflip, UseNearClip);
		} else if(findMedu == 0)
		{
			numMedu++;
		t->SGL_ATTR=ATTRIBUTE(Dual_Plane,SORT_CEN, i, 0, No_Gouraud,
		Window_In|MESHon|HSSon|ECdis|SPdis|CL64Bnk,sprNoflip, UseNearClip);
		} else {
			numNormal++;
		t->SGL_ATTR=ATTRIBUTE(Single_Plane,SORT_CEN, i, 0, No_Gouraud,
		Window_In|MESHoff|HSSon|ECdis|SPdis|CL64Bnk,sprNoflip, UseNearClip);
		}
		
    }
	
		cout << "\n " << numNormal << " normal textures ";
		cout << "\n " << numMesh << " mesh textures ";
		cout << "\n " << numDual << " dual plane textures ";
		cout << "\n " << numMedu << " mesh & dual plane textures \n";
		
		cout << "\n " << number_of_unique_items << " unique items";
		cout << "\n " << number_of_items << " total items \n";
}

void LoadOBJ(string inFolder, string fileIn, animated_model_t * aModel)
{
    cout << "\nNow loading : " << fileIn << "\n";
    string fileInName = inFolder + fileIn + ".obj";
    string fileOutName = "OUT/" + fileIn;

    ifstream file(fileInName, ios::in | ios::ate);
    if (!file.is_open()) {cout << "ERROR READING OBJ FILE\n"; exit(-1);}

    aModel->nbTextures=0;
    aModel->texture = (texture_t*)calloc(256,sizeof(texture_t));

    cout << "Preparing to open OBJ file...\n";
    if (load_OBJ_to_mesh(&file, aModel, -1)) {cout << "\nSuccess : " << fileIn << " loaded...\n";}
    else {cout << "\nERROR : Couldn't load Obj\n"; return;}
    file.close();

	ReadPaletteFile();

    for (unsigned int i=0; i<aModel->nbTextures; i++){
		ReadTGAFile(inFolder, &aModel->texture[i]);
		}

    uint32_t totVertices = 0;
    for (uint32_t i=0; i<aModel->nbModels; i++)
    totVertices+=aModel->model[i].nbPoint;

    for (uint32_t i=0; i<aModel->nbFrames; i++)
    {
        aModel->keyFrames[i].cVert = new vertex_t [totVertices];
        char s[5];
        sprintf(s, "%06d", i+1);
        string postVal(s);
        string animName = inFolder + fileIn + "_" + postVal + ".obj";
        cout << animName << "\n\n****************\n";
        ifstream animFile(animName, ios::in | ios::ate);
        if (!animFile.is_open()) {cout << "ERROR READING OBJ FILE\n"; aModel->nbFrames=i; break;}
        if (load_OBJ_to_mesh(&animFile, aModel, i)) {cout << "\nKeyFrame No. " << i << " loaded...\n";}
        else {cout << "\nERROR : Couldn't load keyframe\n"; break;}
        animFile.close();
    }

    specialConditions(0, aModel->nbTextures, aModel);

    void write_model_binary(ofstream * file, animated_model_t * aModel);
    cout << "NOW CREATING BINARY FILE " << fileOutName << ".GVP\n";
    string binaryFileName = fileOutName + ".GVP";  //Great Value Model [adapted from ZTP]

    ofstream binFile(binaryFileName.c_str(), ios::out | ios::binary);
    if (!binFile.is_open()) {cout << "\nERROR WHILE WRITING BINARY FILE!\n"; return;}
    write_model_binary(&binFile, aModel);
    binFile.close();


}




animated_model_t * loading(void)
{

    string path;

    cout << "SEGA SATURN GREAT VALUE ENGINE MODEL CONVERTER 0.WHAT\n";
    cout << "BY XL2 (2018-05-28) \n Modified by Ponut64 202X \n";

    animated_model_t * newModel = new animated_model_t;
    ZeroMemory(newModel, sizeof(animated_model_t));
    cout << "\n***STEP 1***";
    cout << "\n   Enter how many keyframes your model has\n";
    cout <<"   Example : for 6 keyframes to interpolate your animations, enter 6.\n   0 for no animation.\n\n";
    cout << "ENTER TOTAL KEYFRAMES : ";
    cin >> newModel->nbFrames;

    cout << "\n***STEP 2***";
    cout << "\n   You will now have to enter the folder's relative path\n   IMPORTANT : Don't forget to end it with a '/'.\n   Example : IN/TITLE/  and then press enter.\n\n";
    cout << "ENTER THE PATH : ";
    cin >> path;
    cout << "\n";

    string fileName;
    cout << "\n***STEP 3***\n";
    cout << "\n   You will now have to enter the OBJ filename WITHOUT the extension.\n   Example : TITLE and then press enter.\n\n";
    cout << "ENTER THE NAME : ";
    cin >> fileName;

    newModel->nbModels = 0;
    newModel->model = new model_t[256]; //Temp number
    ZeroMemory(newModel->model, sizeof(model_t) * 256);
    newModel->keyFrames = new key_frame_t[newModel->nbFrames];
    ZeroMemory(newModel->keyFrames, sizeof(key_frame_t) * newModel->nbFrames);
    LoadOBJ(path, fileName, newModel);

    cout << "End of program. Enjoy!\n\n";

    return newModel;
}
