// openclscan - scan for OpenCL devices and report capabilities
// Based on OpenCL 1.0.48 
// Copyright 2010 - Jeff Buck
//
// Home is at http://github.com/itfrombit/openclscan

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <OpenCL/opencl.h>

const int MAX_DEVICE_COUNT = 32;

#define BIT_CHECK_P(var, bit) \
{ \
	if ((var) & (bit)) \
		printf(" " #bit); \
}

static char g_s[1024];
static char g_format[16];
static char g_formatString[256];

#define CL_DEV_INFO_P(device, param, type, desc) \
{ \
	int r; \
	size_t size; \
	type	v##param; \
	if (!strcmp(#type, "char")) \
		r = clGetDeviceInfo(device, param, sizeof(g_s), g_s, &size); \
	else \
		r = clGetDeviceInfo(device, param, sizeof(v##param), &v##param, &size); \
	\
	if (!strcmp(#type, "cl_uint")) \
		strcpy(g_format, "u"); \
	else if (!strcmp(#type, "size_t")) \
		strcpy(g_format, "zd"); \
	else if (!strcmp(#type, "cl_ulong")) \
		strcpy(g_format, "llu"); \
	else if (!strcmp(#type, "cl_bool")) \
		strcpy(g_format, "s"); \
	else if (!strcmp(#type, "char")) \
		strcpy(g_format, "s"); \
	\
	sprintf(g_formatString, "%%-35s: %%%s\n", g_format); \
	if (!strcmp(#type, "cl_bool")) \
		printf(g_formatString, desc, (v##param == CL_TRUE) ? "Yes" : "No"); \
	else if (!strcmp(#type, "cl_ulong")) \
		printf(g_formatString, desc, (v##param >> 20)); \
	else if (!strcmp(#type, "char")) \
		printf(g_formatString, desc, g_s); \
	else \
		printf(g_formatString, desc, v##param); \
}

int queryCLDevices(cl_device_type deviceType, int verbose, int rawOutput)
{
	cl_int				r;
	cl_device_id		deviceList[MAX_DEVICE_COUNT];
	cl_uint				deviceCount;

	r = clGetDeviceIDs(NULL, deviceType, MAX_DEVICE_COUNT, deviceList, &deviceCount);

	if (!rawOutput)
		printf("OpenCL device count: %d\n", deviceCount);

	if (!verbose && !rawOutput)
	{		
		printf("  %-6s %-20s %-s\n", "Type", "Vendor", "Device");
		printf("  %-6s %-20s %-s\n", "----", "------", "------");
	}

	for (int i = 0; i < deviceCount; i++)
	{
		cl_char			vendor[1024];
		cl_char			name[1024];
		cl_device_type	type;

		char*			typeString;
		char*			formatString;

		size_t	size = 0;

		r = clGetDeviceInfo(deviceList[i], CL_DEVICE_VENDOR, sizeof(vendor), vendor, &size);
		r = clGetDeviceInfo(deviceList[i], CL_DEVICE_NAME, sizeof(name), name, &size);
		r = clGetDeviceInfo(deviceList[i], CL_DEVICE_TYPE, sizeof(type), &type, &size);

		switch (type)
		{
		case CL_DEVICE_TYPE_CPU:
			typeString = "CPU";
			break;
			
		case CL_DEVICE_TYPE_GPU:
			typeString = "GPU";
			break;

		default:
			typeString = "Other";
			break;
		}

		if (!verbose)
		{			
			if (rawOutput)
				formatString = "%s|%s|%s\n";
			else
				formatString = "  %-6s %-20s %-s\n";

			printf(formatString, typeString, vendor, name);
		}
		else
		{
			printf("---------------------------------------------------------------------\n");
			printf("%-35s: %s\n", "Device", name);
			printf("%-35s: %s\n", "Type", typeString);
			printf("%-35s: %s\n", "Vendor", vendor);
			printf("\n");

			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_MAX_COMPUTE_UNITS, cl_uint, "Max Compute Units");

			cl_uint			maxWorkItemDimensions;
			r = clGetDeviceInfo(deviceList[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, 
								sizeof(maxWorkItemDimensions),
								&maxWorkItemDimensions, &size);

			size_t* maxWorkItemSizes = malloc(sizeof(size_t) * maxWorkItemDimensions);
			r = clGetDeviceInfo(deviceList[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, 
								sizeof(size_t) * maxWorkItemDimensions,
								maxWorkItemSizes, &size);
			printf("%-35s: [", "Max Work Item Dimensions");
			for (int j = 0; j < maxWorkItemDimensions; j++)
			{
				printf("%u", (unsigned int)maxWorkItemSizes[j]);
				if (j < (maxWorkItemDimensions - 1))
					printf(" ");
			}
			printf("]\n");

			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, size_t, "Max Work Group Size");
			printf("\n");

			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, cl_uint, "Preferred Vector Width - char");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, cl_uint, "Preferred Vector Width - short");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, cl_uint, "Preferred Vector Width - int");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, cl_uint, "Preferred Vector Width - long");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, cl_uint, "Preferred Vector Width - float");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, cl_uint, "Preferred Vector Width - double");

			printf("\n");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, cl_uint, "Max Clock Frequency (MHz)");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_ADDRESS_BITS, cl_uint, "Address Bits");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_MAX_MEM_ALLOC_SIZE, cl_ulong, "Max Memory Alloc Size (MB)");
			printf("\n");

			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_IMAGE_SUPPORT, cl_bool, "Image Support");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_MAX_READ_IMAGE_ARGS, cl_uint, "Max Read Image Args");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_MAX_WRITE_IMAGE_ARGS, cl_uint, "Max Write Image Args");
	
			printf("\n");

			size_t			image2dMaxWidth;
			size_t			image2dMaxHeight;

			size_t			image3dMaxWidth;
			size_t			image3dMaxHeight;
			size_t			image3dMaxDepth;

			r = clGetDeviceInfo(deviceList[i], CL_DEVICE_IMAGE2D_MAX_WIDTH, 
								sizeof(image2dMaxWidth),
								&image2dMaxWidth, &size);

			r = clGetDeviceInfo(deviceList[i], CL_DEVICE_IMAGE2D_MAX_HEIGHT, 
								sizeof(image2dMaxHeight),
								&image2dMaxHeight, &size);

			r = clGetDeviceInfo(deviceList[i], CL_DEVICE_IMAGE3D_MAX_WIDTH, 
								sizeof(image3dMaxWidth),
								&image3dMaxWidth, &size);

			r = clGetDeviceInfo(deviceList[i], CL_DEVICE_IMAGE3D_MAX_HEIGHT, 
								sizeof(image3dMaxHeight),
								&image3dMaxHeight, &size);

			r = clGetDeviceInfo(deviceList[i], CL_DEVICE_IMAGE3D_MAX_DEPTH, 
								sizeof(image3dMaxDepth),
								&image3dMaxDepth, &size);

			printf("%-35s: [%zd, %zd]\n", "2D Image Max Width/Height", 
						image2dMaxWidth, image2dMaxHeight);
			printf("%-35s: [%zd, %zd, %zd]\n", "3D Image Max Width/Height/Depth", 
						image3dMaxWidth, image3dMaxHeight, image3dMaxDepth);
			printf("\n");

			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_MAX_SAMPLERS, cl_uint, "Max Samplers");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_MAX_PARAMETER_SIZE, size_t, "Max Parameter Size");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_MEM_BASE_ADDR_ALIGN, cl_uint, "Memory Base Address Align (bits)");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, cl_uint, "Min Data TypeAlign Size (bytes)");
			printf("\n");

			cl_device_fp_config fpConfig;
			r = clGetDeviceInfo(deviceList[i], CL_DEVICE_SINGLE_FP_CONFIG, 
								sizeof(fpConfig),
								&fpConfig, &size);

			printf("%-35s:", "FP Capabilities");
			BIT_CHECK_P(fpConfig, CL_FP_DENORM);
			BIT_CHECK_P(fpConfig, CL_FP_INF_NAN);
			BIT_CHECK_P(fpConfig, CL_FP_ROUND_TO_NEAREST);
			BIT_CHECK_P(fpConfig, CL_FP_ROUND_TO_ZERO);
			BIT_CHECK_P(fpConfig, CL_FP_ROUND_TO_INF);
			BIT_CHECK_P(fpConfig, CL_FP_FMA);
			printf("\n");
			printf("\n");

			cl_device_mem_cache_type globalMemCacheType;
			r = clGetDeviceInfo(deviceList[i], CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, 
								sizeof(globalMemCacheType),
								&globalMemCacheType, &size);

			printf("%-35s:", "Global Memory Cache Type");
			if (globalMemCacheType & CL_READ_ONLY_CACHE)
				printf(" CL_READ_ONLY_CACHE");
			else if (globalMemCacheType & CL_READ_WRITE_CACHE)
				printf(" CL_READ_WRITE_CACHE");
			else
				printf(" CL_NONE");
			printf("\n");

			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, cl_uint, "Global Mem Cacheline Size");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, cl_ulong, "Global Mem Cache Size");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_GLOBAL_MEM_SIZE, cl_ulong, "Global Memory Size (MB)");

			printf("\n");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, cl_ulong, "Max Constant Buffer Size");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_MAX_CONSTANT_ARGS, cl_uint, "Max Constant Args in a kernel");
			printf("\n");

			cl_device_local_mem_type localMemType;
			r = clGetDeviceInfo(deviceList[i], CL_DEVICE_LOCAL_MEM_TYPE, 
								sizeof(localMemType),
								&localMemType, &size);

			printf("%-35s:", "Local Memory Type");
			BIT_CHECK_P(localMemType, CL_LOCAL);
			BIT_CHECK_P(localMemType, CL_GLOBAL);
			printf("\n");

			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_LOCAL_MEM_SIZE, cl_ulong, "Local Memory Size (bytes)");
			printf("\n");
		
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_ERROR_CORRECTION_SUPPORT, cl_bool, "Error Correction Support");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_PROFILING_TIMER_RESOLUTION, size_t, "Profiling Timer Resolution (ns)");
			printf("\n");

			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_ENDIAN_LITTLE, cl_bool, "Little Endian?");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_AVAILABLE, cl_bool, "Device Available?");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_COMPILER_AVAILABLE, cl_bool, "Device Compiler Available?");

			cl_device_exec_capabilities execCapabilities;
			r = clGetDeviceInfo(deviceList[i], CL_DEVICE_EXECUTION_CAPABILITIES, 
								sizeof(execCapabilities),
								&execCapabilities, &size);

			printf("%-35s:", "Execution Capabilities");
			BIT_CHECK_P(execCapabilities, CL_EXEC_KERNEL);
			BIT_CHECK_P(execCapabilities, CL_EXEC_NATIVE_KERNEL);
			printf("\n");

			cl_command_queue_properties queueProperties;
			r = clGetDeviceInfo(deviceList[i], CL_DEVICE_QUEUE_PROPERTIES, 
								sizeof(queueProperties),
								&queueProperties, &size);

			printf("%-35s:", "Command Queue Properties");
			BIT_CHECK_P(queueProperties, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);
			BIT_CHECK_P(queueProperties, CL_QUEUE_PROFILING_ENABLE)
			printf("\n");

			CL_DEVICE_PLATFORM; //TODO	

			printf("\n");
			CL_DEV_INFO_P(deviceList[i], CL_DRIVER_VERSION, char, "Driver Version");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_VERSION, char, "Device Version");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_PROFILE, char, "Device Profile");
			CL_DEV_INFO_P(deviceList[i], CL_DEVICE_EXTENSIONS, char, "Device Extensions");

			free(maxWorkItemSizes);
		}
	}

	if (!verbose && !rawOutput)
		printf("\nSee detailed device information by running with the -v option.\n\n");

	return 0;
}


void usage(char* name)
{
	printf("usage: %s [-acgrv?]\n", name);
	printf("  -a     Show all devices [default]\n");
	printf("  -c     Only show CPU devices\n");
	printf("  -g     Only show GPU devices\n");
	printf("  -r     Report in raw form with no headers\n");
	printf("  -v     Verbose report - show all clGetDeviceInfo() parameter values\n");
}


int main(int argc, char* argv[])
{
	cl_device_type	deviceType = 0;
	int				rawOutput = 0;
	int				verbose = 0;

	char	ch;
	while ((ch = getopt(argc, argv, "acgrv?")) != -1)
	{
		switch (ch) 
		{
			case 'a':
				deviceType = CL_DEVICE_TYPE_ALL;
				break;

			case 'c':
				deviceType |= CL_DEVICE_TYPE_CPU;
				break;

			case 'g':
				deviceType |= CL_DEVICE_TYPE_GPU;
				break;

			case 'r':
				rawOutput = 1;
				break;
			
			case 'v':
				verbose = 1;
				break;

			case '?':
			default:
				usage(argv[0]);
				return -1;
				break;
		}
	}
				
	argc -= optind;
	argv += optind;

	if (deviceType == 0)
		deviceType = CL_DEVICE_TYPE_ALL;

	queryCLDevices(deviceType, verbose, rawOutput);

	return 0;
}

