#version 430 
#extension GL_ARB_shader_storage_buffer_object : require
layout(local_size_x = 1, local_size_y = 1) in;	
//layout(binding = 0, offset = 0) uniform atomic_uint ac;

//	for texture handling
//layout(rgba8, binding = 0) uniform image2D img;		//input/output image
//		vec4 col=imageLoad(img_input, pixel_coords);
//		imageStore(img_output, pixel_coords, col);


//local group of shaders




layout (std430, binding=0) volatile buffer shader_data
{ 
  vec4 dataA[1024];
  vec4 dataB[1024];

};

layout (std430, binding=1) volatile buffer uniform_data
{
	double freq;
	double q;
	int filtertype;
	double samplerate;
	double in1;
};

uniform int sizeofbuffer;

double b0a0;
double b1a0;
double b2a0;
double a1a0;
double a2a0;
double in2;
double out1;
double out2;
double M_PI = 3.14159265358979323846;

void setFreqAndQ (double freq, double q)
{
	double M_LOG2 = log (2.0);

	double frequency = max(80., freq);
	bool q_is_bandwidth = true;

	// temp coefficient vars
	double alpha = 1.;
	double a0 = 0.;
	double a1 = 0.;
	double a2 = 0.;
	double b0 = 0.;
	double b1 = 0.;
	double b2 = 0.;

	double omega = 2.0 * M_PI * frequency * (1./samplerate);
	double tsin = sin(float(omega));
	double tcos = cos(float(omega));

	if (filtertype == 2)
	{
		if (q_is_bandwidth)
			alpha = tsin * sinh(float(M_LOG2 * 0.5 * q * omega / tsin));
		else
			alpha = tsin / (2.0 * q);

		b0=tsin * 0.5;
		b1=0.0;
		b2=-tsin * 0.5;
		a0=1.0+alpha;
		a1=-2.0*tcos;
		a2=1.0-alpha;
	}
	else
	{
		if (q_is_bandwidth)
			alpha = tsin * sinh(float(M_LOG2 * 0.5 * q * omega / tsin));
		else
			alpha = tsin / (2.0 * q);

		if (filtertype == 0)
		{
			b0=(1.0-tcos) * 0.5;
			b1=1.0-tcos;
			b2=(1.0-tcos) * 0.5;
			a0=1.0+alpha;
			a1=-2.0*tcos;
			a2=1.0-alpha;
		}
		else if (filtertype == 1)
		{
			b0=(1.0+tcos) * 0.5;
			b1=-(1.0+tcos);
			b2=(1.0+tcos) * 0.5;
			a0=1.0+ alpha;
			a1=-2.0*tcos;
			a2=1.0-alpha;
		}
	}
	
	double invA0 = 1.0 / a0;
	b0a0=float(b0 * invA0);
	b1a0=float(b1 * invA0);
	b2a0=float(b2 * invA0);
	a1a0=float(a1 * invA0);
	a2a0=float(a2 * invA0);
}

double process (double sampl)
{
	double outpu = b0a0 * sampl + b1a0 * in1 + b2a0 * in2 - a1a0 * out1 - a2a0 * out2;
	in2 = in1;
	in1 = sampl;
	out2 = out1;
	out1 = outpu;

	return outpu;
}


void main() 
	{
	uint index = gl_GlobalInvocationID.x;		
	
	//setFreqAndQ(freq, q);
	dataB[index].x = float(process(dataB[index].x));


	//dataB[0].x +=1;
	//atomicAdd(dataB[0].x,1);
	
	
	
	//barrier();
	//
	}