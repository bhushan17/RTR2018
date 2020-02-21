
#define TIME_STEPSIZE2 0.3

__global__ void addforce_Kernel(float3* partcleAcclerationArray, float3 accelaration)
{

	//int i = threadIdx.x;
	//int j = threadIdx.y;
	//int i = blockIdx.x * blockDim.x + threadIdx.x;
	//unsigned int Y = blockIdx.y * blockDim.y + threadIdx.y;
	//unsigned int X = blockIdx.x * blockDim.x + threadIdx.x;
	int thread_no = blockIdx.x * blockDim.x + threadIdx.x;
	
	//unsigned int Y = blockIdx.y * blockDim.y + threadIdx.y;
	//unsigned int X = blockIdx.x * blockDim.x + threadIdx.x;
	
	//int j = threadIdx.y;
	float3 vec;
	/*partcleAcclerationArray[i].x = accelaration[0];
	partcleAcclerationArray[i].y = accelaration[1];
	partcleAcclerationArray[i].z = accelaration[2];*/
	
	vec.x=accelaration.x;
	vec.y=accelaration.y;
	vec.z=accelaration.z;
	//partcleAcclerationArray[i] = vec;
	
	//partcleAcclerationArray[Y * 1 + X] = make_float3(0.0f,-0.5*0.3,0.2*0.3);
	//partcleAcclerationArray[i] = make_float3(0.0f,-0.5*0.3,0.2*0.3);
	
	//if ((thread_no < (120 * 80)))
		partcleAcclerationArray[thread_no] = make_float3(0.0 * TIME_STEPSIZE2, -0.5 * TIME_STEPSIZE2, 0.005 * TIME_STEPSIZE2);
		//partcleAcclerationArray[Y * 120 + X] = make_float3(0.0 * TIME_STEPSIZE2, -0.5 * TIME_STEPSIZE2, 0.2 * TIME_STEPSIZE2);
		//partcleAcclerationArray[Y * 120 + X] = make_float3(accelaration.x, accelaration.y, accelaration.z);
	//partcleAcclerationArray[Y * 120 + X] = vec;
}


void addforceCudaKernel(float3* partcleAcclerationArray, float3 accelaration, int numOfParticles,int particle_width, int particle_height)
{
 
  //dim3 DimBlock=dim3(1000,1000);
  //dim3 DimGrid=dim3(1, 1, 1);
   dim3 DimBlock=dim3(particle_width ,1,1);
  dim3 DimGrid=dim3(particle_height, 1, 1);
  
  //dim3 DimBlock=dim3(8,8,1);
  //dim3 DimGrid=dim3(120 / DimBlock.x, 80/DimBlock.y, 1);
 
  addforce_Kernel<<<DimGrid, DimBlock>>>(partcleAcclerationArray, accelaration);
  
}



__device__ float3 GPUgetParticle(float3* GPUParticles_pos_array,int x, int y, int particle_width) { return GPUParticles_pos_array[y*particle_width + x]; }

__device__ float GPUcrossProduct[3];

__device__ float* GPUcross(const float* v1, const float* v2)
{
	/*GPUcrossProduct[0] = v1[1] * v2[2] - v1[2] * v2[1];
	GPUcrossProduct[1] = v1[2] * v2[0] - v1[0] * v2[2];
	GPUcrossProduct[2] = v1[0] * v2[1] - v1[1] * v2[0];*/
	/*crossProduct[0] = (v1[1] * v2[2] - v1[2] * v2[1]) ;
	crossProduct[1] = (v1[2] * v2[0] - v1[0] * v2[2]);
	crossProduct[2] = (v1[0] * v2[1] - v1[1] * v2[0]);*/
	float crossProduct[3] = {v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0]};
	return crossProduct;
}

__device__ float GPUlength(float* v)
{
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

__device__ int GPUgetParticles_pos_array_index(int x, int y, int particle_width) 
{ 
	return (y*particle_width + x); 
}

__global__ void applyWindForce_Kernel(float3* GPUParticles_pos_array, float3* partcleAcclerationArray,int particle_width,int particle_height,  float3 windForce)
{
		
	//unsigned int X = blockIdx.x * blockDim.x + threadIdx.x;
	//unsigned int Y = blockIdx.y * blockDim.y + threadIdx.y;
	int thread_no = blockIdx.x * blockDim.x + threadIdx.x;
	
	
	float *normal;
	float3 TrianglePoint1, TrianglePoint2, TrianglePoint3;
	float v1[3], v2[3];
	float mass = 1.0;
	if(((thread_no) < ((particle_width * particle_height) - blockDim.x  )) && ((thread_no % blockDim.x) < (blockDim.x-1)))
	{
		/*TrianglePoint1 = GPUgetParticle(GPUParticles_pos_array,X+1,Y,particle_width);
		TrianglePoint2 = GPUgetParticle(GPUParticles_pos_array,X,Y,particle_width);
		TrianglePoint3 = GPUgetParticle(GPUParticles_pos_array,X,Y+1,particle_width);*/
		
		TrianglePoint1 = GPUParticles_pos_array[thread_no + blockDim.x];//  ,X+1,Y,particle_width;
		TrianglePoint2 = GPUParticles_pos_array[thread_no];//  ,X,Y,particle_width;
		TrianglePoint3 = GPUParticles_pos_array[thread_no + 1];// ,X,Y+1,particle_width;
		
		
		v1[0] = TrianglePoint2.x - TrianglePoint1.x;
		v1[1] = TrianglePoint2.y - TrianglePoint1.y;
		v1[2] = TrianglePoint2.z - TrianglePoint1.z;
				
		v2[0] = TrianglePoint3.x - TrianglePoint1.x;
		v2[1] = TrianglePoint3.y - TrianglePoint1.y;
		v2[2] = TrianglePoint3.z - TrianglePoint1.z;
		
		normal = GPUcross(&v1[0], &v2[0]);
		
		float d[3];
		float l = GPUlength(&normal[0]);
		
		d[0] = normal[0]/l;
		d[1] = normal[1]/l;
		d[2] = normal[2]/l;	
		
		float dotproduct = d[0] * windForce.x + d[1] * windForce.y + d[2] * windForce.z;
		float force[3] = {normal[0] * dotproduct,normal[1] * dotproduct,normal[1] * dotproduct} ;
		
		int particalIndex = 0;
			particalIndex = thread_no + blockDim.x ; //GPUgetParticles_pos_array_index(X+1,Y,particle_width);
			
			partcleAcclerationArray[particalIndex].x = partcleAcclerationArray[particalIndex].x + force[0] / mass;
			partcleAcclerationArray[particalIndex].y = partcleAcclerationArray[particalIndex].y + force[1] / mass;
			partcleAcclerationArray[particalIndex].z = partcleAcclerationArray[particalIndex].z + force[2] / mass;
			
			
			particalIndex = thread_no;// GPUgetParticles_pos_array_index(X,Y,particle_width);
			
			partcleAcclerationArray[particalIndex].x = partcleAcclerationArray[particalIndex].x + force[0] / mass;
			partcleAcclerationArray[particalIndex].y = partcleAcclerationArray[particalIndex].y + force[1] / mass;
			partcleAcclerationArray[particalIndex].z = partcleAcclerationArray[particalIndex].z + force[2] / mass;
			
			
			particalIndex = thread_no+1;//GPUgetParticles_pos_array_index(X,Y+1,particle_width);
			
			partcleAcclerationArray[particalIndex].x = partcleAcclerationArray[particalIndex].x + force[0] / mass;
			partcleAcclerationArray[particalIndex].y = partcleAcclerationArray[particalIndex].y + force[1] / mass;
			partcleAcclerationArray[particalIndex].z = partcleAcclerationArray[particalIndex].z + force[2] / mass;
			
				
			
			
			/*TrianglePoint1 = GPUgetParticle(GPUParticles_pos_array,X+1,Y+1,particle_width);
			TrianglePoint2 = GPUgetParticle(GPUParticles_pos_array,X+1,Y,particle_width);
			TrianglePoint3 = GPUgetParticle(GPUParticles_pos_array,X,Y+1,particle_width);*/
			
			
			TrianglePoint1 = GPUParticles_pos_array[thread_no + blockDim.x + 1] ;//,X+1,Y+1,particle_width);
			TrianglePoint2 = GPUParticles_pos_array[thread_no+ blockDim.x ];  //,X+1,Y,particle_width);
			TrianglePoint3 = GPUParticles_pos_array[thread_no + 1]; //,X,Y+1,particle_width);
			
			
			v1[0] = TrianglePoint2.x - TrianglePoint1.x;
			v1[1] = TrianglePoint2.y - TrianglePoint1.y;
			v1[2] = TrianglePoint2.z - TrianglePoint1.z;
			
			v2[0] = TrianglePoint3.x - TrianglePoint1.x;
			v2[1] = TrianglePoint3.y - TrianglePoint1.y;
			v2[2] = TrianglePoint3.z - TrianglePoint1.z;
			
			normal = GPUcross(&v1[0], &v2[0]);
			
			l = GPUlength(&normal[0]);
			d[0] = normal[0]/l;
			d[1] = normal[1]/l;
			d[2] = normal[2]/l;			
			
			dotproduct = d[0] * windForce.x + d[1] * windForce.y + d[2] * windForce.z;
			force[0] = normal[0] * dotproduct; force[1] = normal[1] * dotproduct; force[2] = normal[1] * dotproduct;
			
			
			
			particalIndex = thread_no + blockDim.x + 1;//GPUgetParticles_pos_array_index(X+1,Y+1,particle_width);
			
			
			partcleAcclerationArray[particalIndex].x = partcleAcclerationArray[particalIndex].x + force[0] / mass;
			partcleAcclerationArray[particalIndex].y = partcleAcclerationArray[particalIndex].y + force[1] / mass;
			partcleAcclerationArray[particalIndex].z = partcleAcclerationArray[particalIndex].z + force[2] / mass;
			
			
			particalIndex = thread_no + blockDim.x ;//GPUgetParticles_pos_array_index(X+1,Y,particle_width);
			
			partcleAcclerationArray[particalIndex].x = partcleAcclerationArray[particalIndex].x + force[0] / mass;
			partcleAcclerationArray[particalIndex].y = partcleAcclerationArray[particalIndex].y + force[1] / mass;
			partcleAcclerationArray[particalIndex].z = partcleAcclerationArray[particalIndex].z + force[2] / mass;
			
			
			particalIndex = thread_no+1;//GPUgetParticles_pos_array_index(X,Y+1,particle_width);
			
			partcleAcclerationArray[particalIndex].x = partcleAcclerationArray[particalIndex].x + force[0] / mass;
			partcleAcclerationArray[particalIndex].y = partcleAcclerationArray[particalIndex].y + force[1] / mass;
			partcleAcclerationArray[particalIndex].z = partcleAcclerationArray[particalIndex].z + force[2] / mass;
	}	
	
}


void applyWindForceCudaKernel(float3* GPUParticles_pos_array, float3* partcleAcclerationArray, int particle_width, int particle_height, float3 windForce)
{
	//dim3 DimBlock=dim3(8,8,1);
	//dim3 DimGrid=dim3(particle_width / DimBlock.x, particle_height/DimBlock.y, 1);
	
	dim3 DimBlock=dim3(particle_width ,1,1);
	dim3 DimGrid=dim3( particle_height,1, 1);
	
	
	applyWindForce_Kernel<<<DimGrid, DimBlock>>>(GPUParticles_pos_array,partcleAcclerationArray,particle_width,particle_height,windForce);
}


__global__ void timeStep_Kernel(float3* GPUParticles_pos_array,int2* GPUNeighbourParticlesInddex,float* GPURestDistance,bool* GPUMovableStatus,int TotalThreads)
{
	
	
	//unsigned int X = blockIdx.x * blockDim.x + threadIdx.x;
	//unsigned int Y = blockIdx.y * blockDim.y + threadIdx.y;
	//int index = Y*NoOfwidthparticles+X;
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	//int index =  threadIdx.x;
	//int index = X;
	if(index < TotalThreads)
	{
	//for(int i=0; i < 20; i++)
	//{
		float p1_to_p2[3];
		p1_to_p2[0] = GPUParticles_pos_array[GPUNeighbourParticlesInddex[index].y].x - GPUParticles_pos_array[GPUNeighbourParticlesInddex[index].x].x;
		p1_to_p2[1] = GPUParticles_pos_array[GPUNeighbourParticlesInddex[index].y].y - GPUParticles_pos_array[GPUNeighbourParticlesInddex[index].x].y;
		p1_to_p2[2] = GPUParticles_pos_array[GPUNeighbourParticlesInddex[index].y].z - GPUParticles_pos_array[GPUNeighbourParticlesInddex[index].x].z;
												
		float current_distance = GPUlength(&p1_to_p2[0]); // current distance between p1 and p2
				
					
		float correctionVector[3];
		correctionVector[0] = p1_to_p2[0] * (1 - GPURestDistance[index] / current_distance);
		correctionVector[1] = p1_to_p2[1] * (1 - GPURestDistance[index] / current_distance);
		correctionVector[2] = p1_to_p2[2] * (1 - GPURestDistance[index] / current_distance);
				
		// Lets make it half that length, so that we can move BOTH p1 and p2.
		//vec3 correctionVectorHalf = correctionVector * 0.5; 
		float correctionVectorHalf[3];
		correctionVectorHalf[0] = correctionVector[0] * 0.5;
		correctionVectorHalf[1] = correctionVector[1] * 0.5;
		correctionVectorHalf[2] = correctionVector[2] * 0.5;
				
				
		// correctionVectorHalf is pointing from p1 to p2, so the length should move p1 half the length needed to satisfy the constraint.
		if(GPUMovableStatus[GPUNeighbourParticlesInddex[index].x])
		{
			//p1->offsetPos(correctionVectorHalf); 
			GPUParticles_pos_array[GPUNeighbourParticlesInddex[index].x].x = GPUParticles_pos_array[GPUNeighbourParticlesInddex[index].x].x + correctionVectorHalf[0];
			GPUParticles_pos_array[GPUNeighbourParticlesInddex[index].x].y = GPUParticles_pos_array[GPUNeighbourParticlesInddex[index].x].y + correctionVectorHalf[1];
			GPUParticles_pos_array[GPUNeighbourParticlesInddex[index].x].z = GPUParticles_pos_array[GPUNeighbourParticlesInddex[index].x].z + correctionVectorHalf[2];
		}
				
		if(GPUMovableStatus[GPUNeighbourParticlesInddex[index].y])
		{
			//p2->offsetPos(-correctionVectorHalf); 
			GPUParticles_pos_array[GPUNeighbourParticlesInddex[index].y].x = GPUParticles_pos_array[GPUNeighbourParticlesInddex[index].y].x - correctionVectorHalf[0];
			GPUParticles_pos_array[GPUNeighbourParticlesInddex[index].y].y = GPUParticles_pos_array[GPUNeighbourParticlesInddex[index].y].y - correctionVectorHalf[1];
			GPUParticles_pos_array[GPUNeighbourParticlesInddex[index].y].z = GPUParticles_pos_array[GPUNeighbourParticlesInddex[index].y].z - correctionVectorHalf[2];
		}
	//}
	
	
	
	}
	//__syncthreads();
	
}




void timeStepCudaKernel(float3* GPUParticles_pos_array,int2* GPUNeighbourParticlesInddex,float* GPURestDistance,bool* GPUMovableStatus, int NoOfwidthparticles,int NoOfheightparticle, int ToatalThreads)
{

	//dim3 DimBlock=dim3(8,8,1);
	//dim3 DimGrid=dim3(NoOfwidthparticles / DimBlock.x, NoOfheightparticle/DimBlock.y, 1);
	
	dim3 DimBlock=dim3(NoOfwidthparticles,1,1);
	dim3 DimGrid=dim3(NoOfheightparticle, 1, 1);
	
	
		timeStep_Kernel<<<DimGrid, DimBlock>>>(GPUParticles_pos_array,GPUNeighbourParticlesInddex,GPURestDistance,GPUMovableStatus,ToatalThreads);
	

}
	

__global__ void timeStepDisplacement_Kernel( float3* KParticles_pos_array,float3* KParticles_old_pos_array,bool* GPUMovableStatus,float dampingFactor, float3* partcleArray,int particle_width, int particle_height)
{

	int index = blockIdx.x * blockDim.x + threadIdx.x;
	
	if(GPUMovableStatus[index])
	{
		float temp[3];
		temp[0] = KParticles_pos_array[index].x;
		temp[1] = KParticles_pos_array[index].y;
		temp[2] = KParticles_pos_array[index].z;
		
		KParticles_pos_array[index].x = KParticles_pos_array[index].x + (KParticles_pos_array[index].x - KParticles_old_pos_array[index].x)*(1.0 - dampingFactor) + partcleArray[index].x * TIME_STEPSIZE2;
		KParticles_pos_array[index].y = KParticles_pos_array[index].y + (KParticles_pos_array[index].y - KParticles_old_pos_array[index].y)*(1.0 - dampingFactor) + partcleArray[index].y * TIME_STEPSIZE2;
		KParticles_pos_array[index].z = KParticles_pos_array[index].z + (KParticles_pos_array[index].z - KParticles_old_pos_array[index].z)*(1.0 - dampingFactor) + partcleArray[index].z * TIME_STEPSIZE2;
				
		KParticles_old_pos_array[index].x = temp[0];
		KParticles_old_pos_array[index].y = temp[1];
		KParticles_old_pos_array[index].z = temp[2];
		
		partcleArray[index].x = 0.0f;
		partcleArray[index].y = 0.0f;
		partcleArray[index].z = 0.0f;
				
	}
			


}



void timeStepDisplacementCudaKernel(float3* KParticles_pos_array,float3* KParticles_old_pos_array,bool* GPUMovableStatus,float dampingFactor, float3* partcleArray,int particle_width, int particle_height)
{

	dim3 DimBlock=dim3(particle_width,1,1);
	dim3 DimGrid=dim3(particle_height, 1, 1);
	
	timeStepDisplacement_Kernel<<<DimGrid, DimBlock>>>(KParticles_pos_array,KParticles_old_pos_array,GPUMovableStatus,dampingFactor, partcleArray,particle_width, particle_height);

}

	


__global__ void calculateNormal_Kernel(float3* KParticles_pos_array,float3* KParticles_Normal, int Kparticle_width, int Kparticle_height)
{

	int thread_no = blockIdx.x * blockDim.x + threadIdx.x;
	
	
	float *normal;
	float3 TrianglePoint1, TrianglePoint2, TrianglePoint3;
	float v1[3], v2[3];
	
	if(((thread_no) < ((Kparticle_width * Kparticle_height) - blockDim.x  )) && ((thread_no % blockDim.x) < (blockDim.x-1)))
	{
		
		TrianglePoint1 = KParticles_pos_array[thread_no ];  //getParticle(x+1,y);
		TrianglePoint2 = KParticles_pos_array[thread_no + blockDim.x];  //getParticle(x,y);
		TrianglePoint3 = KParticles_pos_array[thread_no + 1];   //getParticle(x,y+1);
			
			
		v1[0] = TrianglePoint2.x - TrianglePoint1.x;
		v1[1] = TrianglePoint2.y - TrianglePoint1.y;
		v1[2] = TrianglePoint2.z - TrianglePoint1.z;
			
		v2[0] = TrianglePoint3.x - TrianglePoint1.x;
		v2[1] = TrianglePoint3.y - TrianglePoint1.y;
		v2[2] = TrianglePoint3.z - TrianglePoint1.z;
			
		normal = GPUcross(&v1[0], &v2[0]);
		
		int particalIndex = 0;
		particalIndex = thread_no  ;
			
		KParticles_Normal[particalIndex].x = normal[0];
		KParticles_Normal[particalIndex].y = normal[1];
		KParticles_Normal[particalIndex].z = normal[2];
		
		particalIndex = thread_no + blockDim.x;
		KParticles_Normal[particalIndex].x = normal[0];
		KParticles_Normal[particalIndex].y = normal[1];
		KParticles_Normal[particalIndex].z = normal[2];
			
		particalIndex = thread_no+1;	
		KParticles_Normal[particalIndex].x = normal[0];
		KParticles_Normal[particalIndex].y = normal[1];
		KParticles_Normal[particalIndex].z = normal[2];
			
			
		normal = NULL;
		//TrianglePoint1 = NULL; TrianglePoint2 = NULL; TrianglePoint3=NULL;
		v1[0] = 0.0f; v1[1] = 0.0f; v1[2] = 0.0f;
		v2[0] = 0.0f; v2[1] = 0.0f; v2[2] = 0.0f;
		


		TrianglePoint1 = KParticles_pos_array[thread_no + 1];  //getParticle(x+1,y+1);
		TrianglePoint2 = KParticles_pos_array[thread_no+ blockDim.x ];  //getParticle(x+1,y);
		TrianglePoint3 = KParticles_pos_array[thread_no + blockDim.x + 1];   //getParticle(x,y+1);
		
					
		v1[0] = TrianglePoint2.x - TrianglePoint1.x;
		v1[1] = TrianglePoint2.y - TrianglePoint1.y;
		v1[2] = TrianglePoint2.z - TrianglePoint1.z;
			
		v2[0] = TrianglePoint3.x - TrianglePoint1.x;
		v2[1] = TrianglePoint3.y - TrianglePoint1.y;
		v2[2] = TrianglePoint3.z - TrianglePoint1.z;
			
		normal = GPUcross(&v1[0], &v2[0]);
		
		particalIndex = thread_no  + 1;
			
		KParticles_Normal[particalIndex].x = normal[0];
		KParticles_Normal[particalIndex].y = normal[1];
		KParticles_Normal[particalIndex].z = normal[2];
		
		particalIndex =  thread_no + blockDim.x;
		KParticles_Normal[particalIndex].x = normal[0];
		KParticles_Normal[particalIndex].y = normal[1];
		KParticles_Normal[particalIndex].z = normal[2];
			
		particalIndex = thread_no + blockDim.x+ 1 ;	
		KParticles_Normal[particalIndex].x = normal[0];
		KParticles_Normal[particalIndex].y = normal[1];
		KParticles_Normal[particalIndex].z = normal[2];
			
	
	}



	


}

void calculateNormalCudaKernel(float3* KParticles_pos_array,float3* KParticles_Normal, int Kparticle_width, int Kparticle_height)
{
	dim3 DimBlock=dim3( Kparticle_width ,1,1);
	dim3 DimGrid=dim3( Kparticle_height, 1, 1);

	calculateNormal_Kernel<<<DimGrid, DimBlock>>>(KParticles_pos_array, KParticles_Normal, Kparticle_width, Kparticle_height);
}	


__global__  void render_Kernel(float3* KtriangleVertices,float3* KtriangleVertices_normal, float3* KParticles_pos_array, float3* KParticles_Normal, int Kparticle_width, int Kparticle_height)
{

	int thread_no = blockIdx.x * blockDim.x + threadIdx.x;
	
	int index = thread_no * 6;
	
	
	if(((thread_no) < ((Kparticle_width * Kparticle_height) - blockDim.x  )) && ((thread_no % blockDim.x) < (blockDim.x-1)))
	{
	float3 point = KParticles_pos_array[ thread_no  ]; //getParticle(x, y + 1);
	float3 pointNormal = KParticles_Normal[ thread_no  ]; //getParticleNormals(x, y+1);
	
	
	KtriangleVertices[index].x = point.x;
	KtriangleVertices[index].y = point.y;
	KtriangleVertices[index].z = point.z;
	
	KtriangleVertices_normal[index].x = pointNormal.x;
	KtriangleVertices_normal[index].y = pointNormal.y;
	KtriangleVertices_normal[index].z = pointNormal.z;
	
	index++;
	
	point = KParticles_pos_array[ thread_no + blockDim.x ]; //getParticle(x, y);
	pointNormal = KParticles_Normal[ thread_no + blockDim.x  ]; //getParticleNormals(x, y);
	
	
	KtriangleVertices[index].x = point.x;
	KtriangleVertices[index].y = point.y;
	KtriangleVertices[index].z = point.z;
	
	KtriangleVertices_normal[index].x = pointNormal.x;
	KtriangleVertices_normal[index].y = pointNormal.y;
	KtriangleVertices_normal[index].z = pointNormal.z;
	
	index++;
	
	
	point = KParticles_pos_array[ thread_no + 1 ]; //getParticle(x + 1, y);
	pointNormal = KParticles_Normal[ thread_no + 1 ]; //getParticleNormals(x + 1, y);
	
	
	KtriangleVertices[index].x = point.x;
	KtriangleVertices[index].y = point.y;
	KtriangleVertices[index].z = point.z;
	
	KtriangleVertices_normal[index].x = pointNormal.x;
	KtriangleVertices_normal[index].y = pointNormal.y;
	KtriangleVertices_normal[index].z = pointNormal.z;
	
	index++;
	
	point = KParticles_pos_array[ thread_no + 1 ]; //getParticle(x, y + 1);
	pointNormal = KParticles_Normal[ thread_no + 1 ]; //getParticleNormals(x, y + 1);
	
	
	KtriangleVertices[index].x = point.x;
	KtriangleVertices[index].y = point.y;
	KtriangleVertices[index].z = point.z;
	
	KtriangleVertices_normal[index].x = pointNormal.x;
	KtriangleVertices_normal[index].y = pointNormal.y;
	KtriangleVertices_normal[index].z = pointNormal.z;
	
	index++;
	
	point = KParticles_pos_array[ thread_no + blockDim.x  ]; //getParticle(x + 1, y);
	pointNormal = KParticles_Normal[ thread_no + blockDim.x ]; //getParticleNormals(x + 1, y);
	
	
	KtriangleVertices[index].x = point.x;
	KtriangleVertices[index].y = point.y;
	KtriangleVertices[index].z = point.z;
	
	KtriangleVertices_normal[index].x = pointNormal.x;
	KtriangleVertices_normal[index].y = pointNormal.y;
	KtriangleVertices_normal[index].z = pointNormal.z;
	
	index++;
	
	point = KParticles_pos_array[ thread_no + blockDim.x + 1 ]; //getParticle(x + 1, y);
	pointNormal = KParticles_Normal[ thread_no + blockDim.x + 1 ]; //getParticleNormals(x + 1, y);
	
	
	KtriangleVertices[index].x = point.x;
	KtriangleVertices[index].y = point.y;
	KtriangleVertices[index].z = point.z;
	
	KtriangleVertices_normal[index].x = pointNormal.x;
	KtriangleVertices_normal[index].y = pointNormal.y;
	KtriangleVertices_normal[index].z = pointNormal.z;
	
	//index++;
	}


}

void renderCudaKernel(float3* KtriangleVertices,float3* KtriangleVertices_normal, float3* KParticles_pos_array, float3* KParticles_Normal, int Kparticle_width, int Kparticle_height)
{

	dim3 DimBlock=dim3( Kparticle_width ,1,1);
	dim3 DimGrid=dim3( Kparticle_height, 1, 1);

	render_Kernel<<<DimGrid, DimBlock>>>(KtriangleVertices, KtriangleVertices_normal, KParticles_pos_array, KParticles_Normal, Kparticle_width, Kparticle_height);

}
	