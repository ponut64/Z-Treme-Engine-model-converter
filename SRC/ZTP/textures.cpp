#include "../COMMON.H"
/*****
CREDIT FOR TGA LOADING FUNCTIONS : JOHANNES FETZ, JO ENGINE
DIRTY ADAPTATION BY XL2
******/

/**** Bytes alignment can be problematic, and since I'm only trying to read the file format, I just went all-short to save time  ****/
typedef struct
{
   unsigned short  idlength;
   unsigned short  colourmaptype;
   unsigned short  datatypecode;
   short colourmaporigin;
   short colourmaplength;
   unsigned short  colourmapdepth;
   short x_origin;
   short y_origin;
   short width;
   short height;
   unsigned short  bitsperpixel;
   unsigned short  imagedescriptor;
} TGA_HEADER;

unsigned char * tgaDataArea;
unsigned char * img_data_addr;

TGA_HEADER palette_header;
unsigned char * palette_work_addr;
unsigned char * paletteData;

	unsigned char * palTblRed;
	unsigned char * palTblBlu;
	unsigned char * palTblGrn;
	int totalPal;
	bool palIsLoaded = false;

int ReadTGAHeader(unsigned char * readByte, TGA_HEADER * header)
{

	header->idlength = readByte[0];
	
	header->colourmaptype = readByte[1]; 
	
	header->datatypecode = readByte[2];
	
	header->colourmaplength = (readByte[5] | readByte[6]<<8) * 3;
	
	header->bitsperpixel = readByte[16];
	
	header->height = (readByte[12] | readByte[13]<<8);
	header->width = (readByte[14] | readByte[15]<<8);
	
	unsigned char imdat = header->idlength + header->colourmaplength + 18;
	
	img_data_addr = (unsigned char *)((unsigned int)readByte + imdat);

	return 0;
}

int ReadTGAData(unsigned char * readByte, texture_t * img, unsigned short bits)
{
//We read this way because TGAs are encoded with bottom left as zero, so we unflip that by doing Y-- and X++
	int dataIndex = 0;
    for (int y=img->height-1; y>=0 ; y--)  //This is actually the right order
    {
         for (int x=0; x< (int)img->width; x++)
        {
				img->pixel[(y*img->width)+x].a = readByte[dataIndex];
				dataIndex++;
        }
    }
    return 1;
}

/**If the loading of the texture fails**/
void createFakeTexture(texture_t * t)
{
    t->width = 8;
    t->height = 8;
    t->pixel = new pixel_t[64];
	
	unsigned char	bad_tex_tbl[64] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 6, 6, 1, 1, 1,
	1, 1, 6, 6, 6, 1, 1, 1,
	1, 1, 1, 6, 6, 1, 1, 1,
	1, 1, 1, 6, 6, 1, 1, 1,
	1, 1, 1, 6, 6, 1, 1, 1,
	1, 6, 6, 6, 6, 6, 6, 1,
	1, 1, 1, 1, 1, 1, 1, 1	
	};
	
	for(int p = 0; p < 64; p++)
	{
		t->pixel[p].a = bad_tex_tbl[p];	
	}
	
}

	//////////////////////////////////////////////
	// Ponut64 Addition
	// Writing palette targa to memory in a particular format.
	//////////////////////////////////////////////
void	WritePaletteTables(unsigned char * data, TGA_HEADER info)
{
	
 	totalPal = info.width * info.height;
	cout << "\n Palette has " << totalPal << " colors \n";

	palTblRed = (unsigned char *)calloc(totalPal, 4);
	palTblBlu = (unsigned char *)calloc(totalPal, 4);
	palTblGrn = (unsigned char *)calloc(totalPal, 4);

	for(int i = 0; i < totalPal; i++)
	{
		int dri = i * 3;
		palTblBlu[i] = data[dri];
		palTblGrn[i] = data[dri+1];
		palTblRed[i] = data[dri+2];

	} 
	
}

	//////////////////////////////////////////////
	// Ponut64 Addition
	// Conversion of an RGB texture to the index of the closest color in the palette file.
	//////////////////////////////////////////////
void	ScanImgColor(unsigned char * data, texture_t * tex)
{
	
 	int totalPix = tex->width * tex->height;

	unsigned char * colTblRed = (unsigned char *)calloc(totalPix, 4);
	unsigned char * colTblBlu = (unsigned char *)calloc(totalPix, 4);
	unsigned char * colTblGrn = (unsigned char *)calloc(totalPix, 4);
	for(int i = 0; i < totalPix; i++)
	{
		int dri = i * 3;
		colTblBlu[i] = data[dri];
		colTblGrn[i] = data[dri+1];
		colTblRed[i] = data[dri+2];

	} 
	
	//Now Compare, and find the most similiar color, for every color, to the palette
	unsigned short * palleteCodes = (unsigned short *)calloc(totalPix, 4);
	
	for(int j = 0; j < (totalPix); j++)
	{
	int bestColor = 0;
	int colDif[3] = {0, 0, 0};
	int colDist = 0;
	int bestDist = 300 << 16;
		for(int c = 0; c < (totalPal); c++)
		{

			colDif[0] = (colTblRed[j] - palTblRed[c]);
			colDif[1] = (colTblGrn[j] - palTblGrn[c]);
			colDif[2] = (colTblBlu[j] - palTblBlu[c]);
			
			//Funnily enough, uses distance equation
			//One could weight colors
			colDist = (colDif[0] * colDif[0]) + (colDif[1] * colDif[1]) + (colDif[2] * colDif[2]);
			
			if(colDist < bestDist)
			{
				bestDist = colDist;
				bestColor = c;
			}
			
		}
		
		palleteCodes[j] = bestColor;
	}
	
	//Write back the palette codes 
	for(int p = 0; p < totalPix; p++)
	{
		img_data_addr[p] = palleteCodes[p];
	}
	
}


	//////////////////////////////////////////////
	// Ponut64 Addition
	// Loading of a 24 bit RGB targa file as the palette file.
	//////////////////////////////////////////////
void ReadPaletteFile(void)
{
    string name;
	int size = 0;
	
	paletteData = (unsigned char *)calloc(32768, 1024); //? big ? size ?
	palette_work_addr = paletteData;
	
	name = "palette.tga";
	
	ifstream file(name.c_str(), ios::in | ios::binary | ios::ate);
	
	cout << "\n Palette File Initilization... \n";
	cout << "(for converting 24-bit RGB TGAs to palette codes) \n";
	
    if (!file.is_open())
        {
            cout << "\n ERROR : COULDN'T LOAD PALETTE \n";
			cout << " RGB TEXTURES WILL BE REFUSED NOW \n";
			palIsLoaded = false;
			return;
        }
	
	size = file.tellg();
	file.seekg(0, ios::beg);
	tgaDataArea = (unsigned char *)calloc(size+64, 1);
	file.read((char*)tgaDataArea, size);
    file.close();

    ReadTGAHeader(tgaDataArea, &palette_header);
        cout << "Texture " << name << ", size : " << palette_header.width << "x" << palette_header.height << ", pixel depth : " << palette_header.bitsperpixel << ", RLE = " << palette_header.datatypecode << "\n";

	if(palette_header.bitsperpixel != 24 || (palette_header.datatypecode == 9 || palette_header.datatypecode == 10) )
	{
		cout << "ERROR : PALETTE MUST BE 24BPP, NO RLE \n";
		cout << " RGB TEXTURES WILL BE REFUSED NOW \n";
		palIsLoaded = false;
		return;
	}

	memcpy(paletteData, img_data_addr, size);
	
	WritePaletteTables(paletteData, palette_header);
	
	palIsLoaded = true;
	
	free(tgaDataArea);
}


int ReadTGAFile (string folder, texture_t * t)
{
    string name;
	string newname;
    TGA_HEADER header;
	int size = 0;
	
    cout << "\n Attempting to read TGA t file...\n";

	//////////////////////////////////////////////
	// Ponut64 Addition
	// If the material name had a prefix, remove it when looking for the TGA file name.
	//////////////////////////////////////////////
        std::size_t findDual = t->name.find("DUAL_");  //Dual-planes
        std::size_t findMesh = t->name.find("MESH_");  //Mesh polys
        std::size_t findMedu = t->name.find("MEDU_");  //Mesh + dual plane polys
        std::size_t findDark = t->name.find("DARK_");  //Dark polys
        std::size_t findPort = t->name.find("PORT_");  // Invisible portal-defining polygon
        std::size_t findOccl = t->name.find("OCCL_");  // Invisible occluder-defining polygon
		std::size_t findIndx = t->name.find("INDX_");   // Textures whose names will be texture ID numbers, not file names
		std::size_t findGost = t->name.find("GOST_");   // No collision polygons (for BUILD-type)
		//// Starting to get whacky
		int np = 0;
		std::size_t findParams[256];		// Also textures whose names will be texture ID numbers, not file names
		//Non-portals
		findParams[np++]	= t->name.find("NDMG0_");   // No subdivision, dual-plane, mesh, no collision
		findParams[np++]	= t->name.find("NDM00_");   // No subdivision, dual-plane, mesh, colllision
		findParams[np++]	= t->name.find("ND0G0_");   // No subdivision, dual-plane, opaque, no collision
		findParams[np++]	= t->name.find("N0MG0_");   // No subdivision, single-plane, mesh, no collision
		findParams[np++]	= t->name.find("ND000_");   // No subdivision, dual plane, opaque, collision
		findParams[np++]	= t->name.find("N0M00_");   // No subdivision, single-plane, mesh, collision
		findParams[np++]	= t->name.find("N00G0_");   // No subdivision, single-plane, opaque, no collision
		findParams[np++]	= t->name.find("N0000_");   // No subdivision, single-plane, opaque, collision
		findParams[np++]	= t->name.find("0DMG0_");   // Dividable, dual-plane, mesh, no collision
		findParams[np++]	= t->name.find("0DM00_");   // Dividable, dual-plane, mesh, colllision
		findParams[np++]	= t->name.find("0D0G0_");   // Dividable, dual-plane, opaque, no collision
		findParams[np++]	= t->name.find("00MG0_");   // Dividable, single-plane, mesh, no collision
		findParams[np++]	= t->name.find("0D000_");   // Dividable, dual plane, opaque, collision
		findParams[np++]	= t->name.find("00M00_");   // Dividable, single-plane, mesh, collision
		findParams[np++]	= t->name.find("000G0_");   // Dividable, single-plane, opaque, no collision

		findParams[np++]	= t->name.find("NDML0_");   // No subdivision, dual-plane, mesh, ladder
		findParams[np++]	= t->name.find("NDMC0_");   // No subdivision, dual-plane, mesh, climbable
		findParams[np++]	= t->name.find("ND0L0_");   // No subdivision, dual-plane, opaque, ladder
		findParams[np++]	= t->name.find("ND0C0_");   // No subdivision, dual plane, opaque, climbable
		findParams[np++]	= t->name.find("N0ML0_");   // No subdivision, single-plane, mesh, ladder
		findParams[np++]	= t->name.find("N0MC0_");   // No subdivision, single-plane, mesh, climbable
		findParams[np++]	= t->name.find("N00L0_");   // No subdivision, single-plane, opaque, ladder
		findParams[np++]	= t->name.find("N00C0_");   // No subdivision, single-plane, opaque, climbable
		findParams[np++]	= t->name.find("0DML0_");   // Dividable, dual-plane, mesh, ladder
		findParams[np++]	= t->name.find("0DMC0_");   // Dividable, dual-plane, mesh, climbable
		findParams[np++]	= t->name.find("0D0L0_");   // Dividable, dual-plane, opaque, ladder
		findParams[np++]	= t->name.find("0D0C0_");   // Dividable, dual plane, opaque, climbable
		findParams[np++]	= t->name.find("00ML0_");   // Dividable, single-plane, mesh, ladder
		findParams[np++]	= t->name.find("00MC0_");   // Dividable, single-plane, mesh, climbable
		findParams[np++]	= t->name.find("000L0_");   // Dividable, single-plane, opaque, ladder
		findParams[np++]	= t->name.find("000C0_");   // Dividable, single-plane, opaque, climbable
		//All of the following are portals
		//'P' for portal for portal IN
		findParams[np++]	= t->name.find("NDMGP_");   // No subdivision, dual-plane, mesh, no collision
		findParams[np++]	= t->name.find("NDM0P_");   // No subdivision, dual-plane, mesh, colllision
		findParams[np++]	= t->name.find("ND0GP_");   // No subdivision, dual-plane, opaque, no collision
		findParams[np++]	= t->name.find("N0MGP_");   // No subdivision, single-plane, mesh, no collision
		findParams[np++]	= t->name.find("ND00P_");   // No subdivision, dual plane, opaque, collision
		findParams[np++]	= t->name.find("N0M0P_");   // No subdivision, single-plane, mesh, collision
		findParams[np++]	= t->name.find("N00GP_");   // No subdivision, single-plane, opaque, no collision
		findParams[np++]	= t->name.find("N000P_");   // No subdivision, single-plane, opaque, collision
		findParams[np++]	= t->name.find("0DMGP_");   // Dividable, dual-plane, mesh, no collision
		findParams[np++]	= t->name.find("0DM0P_");   // Dividable, dual-plane, mesh, colllision
		findParams[np++]	= t->name.find("0D0GP_");   // Dividable, dual-plane, opaque, no collision
		findParams[np++]	= t->name.find("00MGP_");   // Dividable, single-plane, mesh, no collision
		findParams[np++]	= t->name.find("0D00P_");   // Dividable, dual plane, opaque, collision
		findParams[np++]	= t->name.find("00M0P_");   // Dividable, single-plane, mesh, collision
		findParams[np++]	= t->name.find("000GP_");   // Dividable, single-plane, opaque, no collision
		//'O' for occluder for PORTAL OUT
		findParams[np++]	= t->name.find("NDMGO_");   // No subdivision, dual-plane, mesh, no collision
		findParams[np++]	= t->name.find("NDM0O_");   // No subdivision, dual-plane, mesh, colllision
		findParams[np++]	= t->name.find("ND0GO_");   // No subdivision, dual-plane, opaque, no collision
		findParams[np++]	= t->name.find("N0MGO_");   // No subdivision, single-plane, mesh, no collision
		findParams[np++]	= t->name.find("ND00O_");   // No subdivision, dual plane, opaque, collision
		findParams[np++]	= t->name.find("N0M0O_");   // No subdivision, single-plane, mesh, collision
		findParams[np++]	= t->name.find("N00GO_");   // No subdivision, single-plane, opaque, no collision
		findParams[np++]	= t->name.find("N000O_");   // No subdivision, single-plane, opaque, collision
		findParams[np++]	= t->name.find("0DMGO_");   // Dividable, dual-plane, mesh, no collision
		findParams[np++]	= t->name.find("0DM0O_");   // Dividable, dual-plane, mesh, colllision
		findParams[np++]	= t->name.find("0D0GO_");   // Dividable, dual-plane, opaque, no collision
		findParams[np++]	= t->name.find("00MGO_");   // Dividable, single-plane, mesh, no collision
		findParams[np++]	= t->name.find("0D00O_");   // Dividable, dual plane, opaque, collision
		findParams[np++]	= t->name.find("00M0O_");   // Dividable, single-plane, mesh, collision
		findParams[np++]	= t->name.find("000GO_");   // Dividable, single-plane, opaque, no collision

		findParams[np++]	= t->name.find("0000O_");   // Dividable, single-plane, opaque, collision, occluder
		findParams[np++]	= t->name.find("0000P_");   // Dividable, single-plane, opaque, collision, portal

		bool found_special = false;
		for(int i = 0; i < np; i++)
		{
			if(findParams[i] == 0)
			{
				found_special = true;
				break;
			}
		}

		if(found_special || findIndx == 0 || findPort == 0 || findOccl == 0)
		{
			cout << t->name + "\n";
			cout << "Texture was determined to be by-index, no texture loaded/generated \n";
			return -1; //Do not add a texture in these cases.
		}
		
        std::size_t findSectorSpecs = t->name.find(";");  // Find the sector specifications of a name
		newname = t->name;
		if(findSectorSpecs != std::string::npos) newname.erase(newname.end()-3, newname.end());
		
		//If a preprocessor is found, remove the preprocessor from the file name
		if(findDual == 0 || findMesh == 0 || findMedu == 0 || findDark == 0 || findGost == 0) {
	newname.erase(newname.begin(), newname.begin()+5);
			}
    name =  folder + newname + ".TGA"; //Temporary : Will break under several conditions


    ifstream ibinfile(name.c_str(), ios::in | ios::binary | ios::ate);

    if (!ibinfile.is_open())
        {
            cout << "ERROR : COULDN'T LOAD FILE " << newname.c_str() << ".TGA, creating a bad t \n";
            createFakeTexture(t);
            return -1;
        }
	size = ibinfile.tellg();
	tgaDataArea = (unsigned char *)calloc(size+64, 1);
	ibinfile.seekg(0, ios::beg);
	ibinfile.read((char*)tgaDataArea, size);
    ibinfile.close();

    ReadTGAHeader(tgaDataArea, &header);
        cout << "t " << newname << ", size : " << header.width << "x" << header.height << ", pixel depth : " << header.bitsperpixel << ", RLE = " << header.datatypecode << "\n";

    if (header.datatypecode == 9 || header.datatypecode == 10) {
            createFakeTexture(t);
            return -1;
		cout << "RLE currently not supported, creating bad t \n";
		}
		
    t->width = header.width;
    t->height = header.height;
	
	//////////////////////////////////////////////
	// Ponut64 Addition
	// If TGA is 24BPP RGB and the palette file is loaded, scan the image and find the best palette IDs to match it.
	// If it is not loaded in that case, create a fake t.
	// If it doesn't otherwise match, create a fake t.
	// If it's an 8bpp indexed color TGA, write its data as the t data raw.
	//////////////////////////////////////////////
	if(header.bitsperpixel == 24 && palIsLoaded == true){
		ScanImgColor(img_data_addr, t);
		cout << "CONVERTING RGB t \n";
	} else if(palIsLoaded == false)
	{
		createFakeTexture(t);
		return -1;
		cout << "No palette was found, an RGB t was found, creating bad t \n";
	} else if(header.bitsperpixel != 8){
		createFakeTexture(t);
		return -1;
		cout << "24BPP RGB or indexed color, no RLE, please \n";
	}

    t->pixel = new pixel_t[header.height*header.width];

    ReadTGAData(img_data_addr, t, header.bitsperpixel);

	free(tgaDataArea);

    return 7777;
}


