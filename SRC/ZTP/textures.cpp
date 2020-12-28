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
	
	header->width = (readByte[12] | readByte[13]<<8);
	header->height = (readByte[14] | readByte[15]<<8);
	
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
	
	for(int p = 0; p < 64; p++)
	{
		if( p & 1)
		{
		t->pixel[p].a = 47;	
		} else {
		t->pixel[p].a = 55;
		}
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


int ReadTGAFile (string folder, texture_t * texture)
{
    string name;
	string newname;
    TGA_HEADER header;
	int size = 0;
	
    cout << "\n Attempting to read TGA texture file...\n";

	//////////////////////////////////////////////
	// Ponut64 Addition
	// If the material name had a prefix, remove it when looking for the TGA file name.
	//////////////////////////////////////////////
        std::size_t findDual = texture->name.find("DUAL_");  //Dual-planes
        std::size_t findMesh = texture->name.find("MESH_");  //Mesh polys
        std::size_t findMedu = texture->name.find("MEDU_");  //Mesh + dual plane polys
		
		if(findDual == 0 || findMesh == 0 || findMedu == 0) //If a preprocessor is found, remove the preprocessor from the file name
		{
	newname = texture->name;
	newname.erase(newname.begin(), newname.begin()+5);
    name =  folder + newname + ".TGA"; //Temporary : Will break under several conditions
		} else {
	newname = texture->name;
    name =  folder + newname + ".TGA"; //Temporary : Will break under several conditions
		}

    ifstream ibinfile(name.c_str(), ios::in | ios::binary | ios::ate);

    if (!ibinfile.is_open())
        {
            cout << "ERROR : COULDN'T LOAD FILE " << newname.c_str() << ".TGA, creating a bad texture \n";
            createFakeTexture(texture);
            return -1;
        }
	size = ibinfile.tellg();
	tgaDataArea = (unsigned char *)calloc(size+64, 1);
	ibinfile.seekg(0, ios::beg);
	ibinfile.read((char*)tgaDataArea, size);
    ibinfile.close();

    ReadTGAHeader(tgaDataArea, &header);
        cout << "Texture " << newname << ", size : " << header.width << "x" << header.height << ", pixel depth : " << header.bitsperpixel << ", RLE = " << header.datatypecode << "\n";

    if (header.datatypecode == 9 || header.datatypecode == 10) {
            createFakeTexture(texture);
            return -1;
		cout << "RLE currently not supported, creating bad texture \n";
		}
		
    texture->width = header.width;
    texture->height = header.height;
	
	//////////////////////////////////////////////
	// Ponut64 Addition
	// If TGA is 24BPP RGB and the palette file is loaded, scan the image and find the best palette IDs to match it.
	// If it is not loaded in that case, create a fake texture.
	// If it doesn't otherwise match, create a fake texture.
	// If it's an 8bpp indexed color TGA, write its data as the texture data raw.
	//////////////////////////////////////////////
	if(header.bitsperpixel == 24 && palIsLoaded == true){
		ScanImgColor(img_data_addr, texture);
		cout << "CONVERTING RGB TEXTURE \n";
	} else if(palIsLoaded == false)
	{
		createFakeTexture(texture);
		return -1;
		cout << "No palette was found, an RGB texture was found, creating bad texture \n";
	} else if(header.bitsperpixel != 8){
		createFakeTexture(texture);
		return -1;
		cout << "24BPP RGB or indexed color, no RLE, please \n";
	}

    texture->pixel = new pixel_t[header.height*header.width];

    ReadTGAData(img_data_addr, texture, header.bitsperpixel);

	free(tgaDataArea);

    return 7777;
}


