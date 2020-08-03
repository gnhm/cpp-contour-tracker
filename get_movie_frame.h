//#include "average_intensity.h"

#define CAMERA_PIXELMODE_MONO_8         1
#define CAMERA_PIXELMODE_MONO_16BE      2 // Big endian
#define CAMERA_PIXELMODE_MONO_16LE      3 // Little endian
#define CAMERA_FEEDBACK_PARAMETER_NUMBER 6 // Maximum number of parameters

//Take a video and a camera_feedback function
//Return a list of actions and when they happened.
//camera_feedback( struct camera_frame_struct *frame, double *parameter )
//
//need function that updates the movie struct

int first_frame(camera_frame_struct *temika_frame)
{
	int index;
	FILE *moviefile;
        long offset;
        bool found;
        uint32_t magic;
        struct camera_save_struct camera_frame;
        struct iidc_save_struct iidc_frame;
        struct andor_save_struct andor_frame;
        struct ximea_save_struct ximea_frame;
        uint32_t size_x, size_y;
	uint8_t *imagebuf;
	int frame = 0;
	double parameter[6];

	parameter[0] = 0.0;
	parameter[1] = 0.0;
	parameter[2] = 0.0;
	parameter[3] = 0.0;
	parameter[4] = 0.0;
	parameter[5] = 0.0;


//	if ( !( moviefile = fopen("/Users/guilherme/Documents/Code/temika-simulator/Ron11_3d7_GFP_invasion.12Sep2019_16.01.17.movie", "rb" ) ) )
	if ( !( moviefile = fopen("/Users/guilherme/Desktop/cell_images/cell_0001.movie", "rb" ) ) )
	{
		printf( "Couldn't open movie file.\n" );
		exit( EXIT_FAILURE );
	}

	// Find the beginning of binary data, it won't work if "TemI" is written in the header.
	offset = 0;
	found = false;
	while ( fread( &magic, sizeof( uint32_t ), 1, moviefile ) == 1 )
	{
		if ( magic == CAMERA_MOVIE_MAGIC ) 
		{
			found = true;
			break;
		}
		offset++;
		fseek( moviefile, offset, SEEK_SET );
	}
	// Go to the beginning of frames
	fseek( moviefile, offset, SEEK_SET );

	if ( found )
	{
		index = 0;

		// Go through all the movie
		while ( fread( &camera_frame, sizeof( struct camera_save_struct ), 1, moviefile ) == 1 ) // Check for the end of the file
		{
			
			frame++;

			if ( camera_frame.magic != CAMERA_MOVIE_MAGIC )
			{
				offset = ftell( moviefile );
				printf( "Wrong magic at offset %lu\n", offset );
				exit( EXIT_FAILURE );
			}
			if ( camera_frame.version != CAMERA_MOVIE_VERSION )
			{
				printf( "Unsuported version %u\n", camera_frame.version );
				exit( EXIT_FAILURE );
			}
			// Go to the beginning of the frame for easier reading
			fseek( moviefile, -sizeof( struct camera_save_struct ), SEEK_CUR );

			// Read the header
			if ( camera_frame.type == CAMERA_TYPE_IIDC )
			{
				if ( fread( &iidc_frame, IIDC_MOVIE_HEADER_LENGTH, 1, moviefile ) != 1 )
				{
					offset = ftell( moviefile );
					printf( "Corrupted header at offset %lu\n", offset );
					exit( EXIT_FAILURE );
				}
				size_x = iidc_frame.i_size_x;
				size_y = iidc_frame.i_size_y;
				iidc_to_frame(&iidc_frame, temika_frame);
			}
			else if ( camera_frame.type == CAMERA_TYPE_ANDOR )
			{
				if ( fread( &andor_frame, ANDOR_MOVIE_HEADER_LENGTH, 1, moviefile ) != 1 )
				{
					offset = ftell( moviefile );
					printf( "Corrupted header at offset %lu\n", offset );
					exit( EXIT_FAILURE );
				}
				size_x = ( andor_frame.a_x_end - andor_frame.a_x_start + 1 ) / andor_frame.a_x_bin;
				size_y = ( andor_frame.a_y_end - andor_frame.a_y_start + 1 ) / andor_frame.a_y_bin;
			}
			else if ( camera_frame.type == CAMERA_TYPE_XIMEA )
			{
				if ( fread( &ximea_frame, XIMEA_MOVIE_HEADER_LENGTH, 1, moviefile ) != 1 )
				{
					offset = ftell( moviefile );
					printf( "Corrupted header at offset %lu\n", offset );
					exit( EXIT_FAILURE );
				}
				size_x = ximea_frame.x_size_x;
				size_y = ximea_frame.x_size_y;
			}
			else
			{
				printf( "Unsuported camera.\n" );
				return -1;
			}

			// Read the data
			imagebuf = (uint8_t*) malloc(camera_frame.length_data);
			if ( fread( imagebuf, camera_frame.length_data, 1, moviefile ) != 1 )
			{
				offset = ftell( moviefile );
				printf( "Corrupted data at offset %lu\n", offset );
				exit( EXIT_FAILURE );
			}

			temika_frame->image = imagebuf;

			free(imagebuf);
			return 0;
		}
	}
	return 0;
}

void image_array(camera_frame_struct *frame, double *im_array)
{
	unsigned int pixel;
        for (int i = 0; i < frame->size_x*frame->size_y; i++ )
	{
                if ( frame->pixelmode == CAMERA_PIXELMODE_MONO_16BE )
		{
                        // Big endian
                        pixel = frame->image[2*i];
                        pixel <<= 8;
                        pixel |= frame->image[2*i+1];
		}
                else if ( frame->pixelmode == CAMERA_PIXELMODE_MONO_16LE )
		{
                        // Little endian
                        pixel = frame->image[2*i+1];
                        pixel <<= 8;
                        pixel |= frame->image[2*i];
		}
                else
		{
                        // 8 bit
                        pixel = frame->image[i];
		}
                im_array[i] = pixel;
        }
}
