#include<stdio.h>
#include<windows.h>
float *objVertices;


int **face_indicies;
float **model_vertices;
float **model_normals;
float **model_textures;

int numberOfVertex = 0;
int numberOfNormals = 0;
int numberOfTextureCoordinate = 0;
int faceIndiciesIndex = 0;

void countIndices(const char* filePath)
{
	FILE *objFile;
	char c[100];
	fopen_s(&objFile, filePath, "r");

	while (fgets(c, 100, objFile))
	{
		for (int i = 0; i < 100; i++)
		{
			if (c[i] == 'v')
			{
				if (c[i + 1] == 't')
					numberOfTextureCoordinate++;
				else if (c[i + 1] == 'n')
					numberOfNormals++;
				else if (c[i + 1] == ' ') {
					numberOfVertex++;
				}
			}
			if (c[i] == 'f')
			{
				faceIndiciesIndex++;
			}
		}
	}

	fclose(objFile);
}

void fillarray(const char* filePath)
{
	FILE *objFile;
	fopen_s(&objFile, filePath, "r");
	char buff[100];
	int num, offset, origin, num1, num2, num3, num4, num5, num6, num7, num8, num9;
	int i = 0, j = 0, k = 0, l = 0;
	float vertex;
	/*fprintf(gpFile, "\n start of Fillarray ");
	fclose(gpFile);
	fopen_s(&gpFile, "Log.txt", "a+");*/

	/*fprintf(gpFile, "\n  Start of while = %d ", l);
	fclose(gpFile);
	fopen_s(&gpFile, "Log.txt", "a+");*/
	//fseek(objFile, 0L, SEEK_SET);
	while (fscanf_s(objFile, "%s", buff, 20))
	{
		/*fprintf(gpFile, "\n  Inside while = %d ", l);
		fclose(gpFile);
		fopen_s(&gpFile, "Log.txt", "a+");*/

		if (strcmp(buff, "v") == 0)
		{
			//printf("\n Inside V");
			fscanf_s(objFile, "%f", &vertex);
			//printf("\nvertex X =%.15f",vertex);
			model_vertices[k][0] = vertex;
			//printf("\n v %f",model_model_vertices[k][0]);


			fscanf_s(objFile, "%f", &vertex);
			//printf("  vertex Y =%.15f",vertex);
			model_vertices[k][1] = vertex;
			//printf(" %f",model_model_vertices[k][1]);

			fscanf_s(objFile, "%f", &vertex);
			//printf(" vertex  Z =%.15f",vertex);
			model_vertices[k][2] = vertex;
			//printf(" %f",model_vertices[k][2]);
			k++;

			/*fprintf(gpFile, "\n Value of k = %d ",k);
			fclose(gpFile);
			fopen_s(&gpFile, "Log.txt", "a+");*/
		}

		if (strcmp(buff, "vt") == 0) {
			//numberOfTextureCoordinate++;
			fscanf_s(objFile, "%f", &vertex);
			//printf("\nText X = %.15f", vertex);
			model_textures[i][0] = vertex;
			//printf("\nText X = %.15f", model_textures[i][0]);

			fscanf_s(objFile, "%f", &vertex);
			//printf("  Text Y =%.15f", vertex);
			model_textures[i][1] = vertex;
			//printf("  Text Y =%.15f", model_textures[i][1]);

			//fscanf_s(objFile,"%lf",&vertex);
			//printf("  Text Y =%.15f",vertex);
			model_textures[i][2] = 0.0;
			//printf("  Text Y =%.15f", model_textures[i][2]);
			i++;

		}
		if (strcmp(buff, "vn") == 0) {
			//numberOfNormals++;
			fscanf_s(objFile, "%f", &vertex);
			//printf("\nNormal X = %.15f",vertex);
			model_normals[j][0] = vertex;

			fscanf_s(objFile, "%f", &vertex);
			//printf("  Normal Y =%.15f",vertex);
			model_normals[j][1] = vertex;

			fscanf_s(objFile, "%f", &vertex);
			//printf(" Normal  Z =%.15f",vertex);
			model_normals[j][2] = vertex;

			j++;
		}

		if (strcmp(buff, "f") == 0)
		{
			//printf("\n Inside F");
			/*fprintf(gpFile, "\n  Inside F = %d ", l);
			fclose(gpFile);
			fopen_s(&gpFile, "Log.txt", "a+");*/
			//faceIndiciesIndex++;
			fscanf_s(objFile, "%d/%d/%d %d/%d/%d %d/%d/%d", &num1, &num2, &num3, &num4, &num5, &num6, &num7, &num8, &num9);
			//printf("\n Vertex X = %d %d %d %d %d %d %d %d %d ",num1,num2,num3,num4,num5,num6,num7,num8,num9);  

			face_indicies[l][0] = num1;
			face_indicies[l][1] = num2;
			face_indicies[l][2] = num3;
			face_indicies[l][3] = num4;
			face_indicies[l][4] = num5;
			face_indicies[l][5] = num6;
			face_indicies[l][6] = num7;
			face_indicies[l][7] = num8;
			face_indicies[l][8] = num9;
			l++;
		}


		if (strcmp(buff, "EOF") == 0) {
			break;
		}
	}
	fclose(objFile);


	/*fprintf(gpFile,"\n End of Fillarray ");
	fclose(gpFile);
	fopen_s(&gpFile, "Log.txt", "a+");*/
}


void createVertexArray()
{
	//printf("\n Inside Createarray ");
	int i = 0, j = 0;


	for (i = 0; i < faceIndiciesIndex - 20; i++)
	{
		//printf("\n Inside Createarray for array %d ", i);
		//printf("\n Face indisec for array %d ", (face_indicies[i][0]));

		//if((face_indicies[i][0]) > 333){
			//break;
		//}
		objVertices[j++] = model_vertices[(face_indicies[i][0]) - 1][0];
		objVertices[j++] = model_vertices[(face_indicies[i][0]) - 1][1];
		objVertices[j++] = model_vertices[(face_indicies[i][0]) - 1][2];

		objVertices[j++] = model_vertices[(face_indicies[i][3]) - 1][0];
		objVertices[j++] = model_vertices[(face_indicies[i][3]) - 1][1];
		objVertices[j++] = model_vertices[(face_indicies[i][3]) - 1][2];

		objVertices[j++] = model_vertices[(face_indicies[i][6]) - 1][0];
		objVertices[j++] = model_vertices[(face_indicies[i][6]) - 1][1];
		objVertices[j++] = model_vertices[(face_indicies[i][6]) - 1][2];

		//printf("\n Inside model_vertices for array %f ", model_vertices[(face_indicies[i][6])][2]);

	}




	int z = 0;
	//printf("\n Object Vertices  complete ");
	for (int p = 0; p < faceIndiciesIndex; p++)
	{
		/*printf("\n objVertices 1 = %f", objVertices[z++]);
		printf("   objVertices 2 = %f ", objVertices[z++]);
		printf("   objVertices 3 = %f ", objVertices[z++]);
		printf("   objVertices 4 = %f ", objVertices[z++]);
		printf("   objVertices 5 = %f ", objVertices[z++]);
		printf("   objVertices 6 = %f ", objVertices[z++]);
		printf("   objVertices 7 = %f ", objVertices[z++]);
		printf("   objVertices 8 = %f ", objVertices[z++]);
		printf("   objVertices 9 = %f ", objVertices[z++]);*/
	}
}



void modelloading()
{
	countIndices("sample.obj");

	/*fprintf(gpFile, "\n numberOfVertex = %d ", numberOfVertex);
	fprintf(gpFile, "\n numberOfTextureCoordinate = %d ", numberOfTextureCoordinate);
	fprintf(gpFile, "\n numberOfNormals = %d ", numberOfNormals);
	fprintf(gpFile, "\n faceIndiciesIndex = %d ", faceIndiciesIndex);
	fclose(gpFile);
	fopen_s(&gpFile, "Log.txt", "a+");*/
	/*printf("\n numberOfTextureCoordinate = %d ", numberOfTextureCoordinate);
	printf("\n numberOfNormals = %d ", numberOfNormals);
	printf("\n faceIndiciesIndex = %d ", faceIndiciesIndex);*/


	model_vertices = (float**)malloc((50 + numberOfVertex) * sizeof(float*));
	for (int i = 0; i < (50 + numberOfVertex); i++)
	{
		model_vertices[i] = (float*)malloc(3 * sizeof(float));
	}

	objVertices = (float*)malloc((50 + faceIndiciesIndex) * 9 * sizeof(float));
	/*for(int i =0; i<numberOfVertex; i++)
	{
		objVertices[i] = (float*)malloc(3 * sizeof(float));
	}*/

	model_normals = (float**)malloc((50 + numberOfNormals) * sizeof(float*));
	for (int i = 0; i < (50 + numberOfNormals); i++)
	{
		model_normals[i] = (float*)malloc(3 * sizeof(float));
	}

	model_textures = (float**)malloc((50 + numberOfTextureCoordinate) * sizeof(float*));
	for (int i = 0; i < (50 + numberOfTextureCoordinate); i++)
	{
		model_textures[i] = (float*)malloc(3 * sizeof(float));
	}

	face_indicies = (int**)malloc((50 + faceIndiciesIndex) * sizeof(int*));
	for (int i = 0; i < (50 + faceIndiciesIndex); i++)
	{
		face_indicies[i] = (int*)malloc(9 * sizeof(int));
	}

	/*fprintf(gpFile, "\n malloc done ");
	fclose(gpFile);
	fopen_s(&gpFile, "Log.txt", "a+");*/
	fillarray("sample.obj");
	/*fprintf(gpFile, "\n Fillarray completed. ");
	fclose(gpFile);
	fopen_s(&gpFile, "Log.txt", "a+");*/
	//printf("\n Fillarray completed. ");
	createVertexArray();

	//return(0);

}