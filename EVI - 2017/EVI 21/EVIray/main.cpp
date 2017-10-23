#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <unistd.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <FreeImage.h>

#include "sampling.hpp"
#include "scene.hpp"
#include "ray.hpp"
#include "tracer.hpp"
#include "path_tracer.hpp"
#include "whitted_tracer.hpp"

using namespace std;
using namespace glm;

////////////////////////////////////////////
// Defines.
////////////////////////////////////////////
#define ANSI_BOLD_YELLOW "\x1b[1;33m"
#define ANSI_RESET_STYLE "\x1b[m"
#define MAX_W 1920
#define MAX_H 1080

////////////////////////////////////////////
// Function prototypes.
////////////////////////////////////////////
static void print_usage(char ** const argv);
static void parse_args(int argc, char ** const argv);

////////////////////////////////////////////
// Constants.
////////////////////////////////////////////
static const char * OUT_FILE = "output.png";

////////////////////////////////////////////
// Global variables.
////////////////////////////////////////////
typedef enum TRACERS { NONE, WHITTED, MONTE_CARLO } tracer_t;

static char * g_input_file = NULL;        // Input scene file name.
static char * g_out_file_name = NULL;     // Output image file name.
static int g_samples = 25;                // Samples per pixel.
static float g_fov = 45.0f;               // Camera field of view.
static int g_w = 640;                     // Image width.
static int g_h = 480;                     // Image height.
static float g_a_ratio = 640.0f / 480.0f; // Image aspect ratio.
static vec3 image[MAX_H][MAX_W];          // Image.
static tracer_t g_tracer = NONE;          // Ray tracer to use.
static unsigned int g_max_depth = 5;      // Max recursion depth.
static float g_gamma = 2.2f;              // Gamma correction factor.
static float g_exposure = 0.0f;           // Exposure correction factor.

////////////////////////////////////////////
// Main function.
////////////////////////////////////////////
int main(int argc, char ** argv) {
  Ray r;                    // Primary ray.
  vec2 sample;              // Pixel sample position.
  Tracer * tracer;          // Tracer to use.
  uint64_t total;           // Total of samples to calculate;
  uint64_t current = 0;     // Samples calculated.
  FIBITMAP * input_bitmap;
  FIBITMAP * output_bitmap;
  FREE_IMAGE_FORMAT fif;
  BYTE * bits;
  FIRGBF *pixel;
  int pitch;
  Scene * scn;

  // Parse command line arguments.
  parse_args(argc, argv);
  
  // Initialize FreeImage.
  FreeImage_Initialise();

  // Try to read a scene file.
  try {
    scn = new Scene(g_input_file);
  } catch (SceneError & e) {
    cout << e.what() << endl;
    return EXIT_FAILURE;
  }

  // Print run parameters.
  cout << "Rendering the input file: " << ANSI_BOLD_YELLOW << g_input_file << ANSI_RESET_STYLE << endl;
  cout << "The scene contains: " << endl;
  cout << "  " << ANSI_BOLD_YELLOW << scn->m_figures.size() << ANSI_RESET_STYLE << (scn->m_figures.size() != 1 ? " figures." : " figure.") << endl;
  cout << "  " << ANSI_BOLD_YELLOW << scn->m_lights.size() << ANSI_RESET_STYLE << " light "  << (scn->m_lights.size() != 1 ? "sources." : "source.") << endl;
  cout << "Output image resolution is " << ANSI_BOLD_YELLOW << g_w << "x" << g_h << ANSI_RESET_STYLE << " pixels." << endl;
  cout << "Using " << ANSI_BOLD_YELLOW << g_samples << ANSI_RESET_STYLE << " samples per pixel." << endl;
  cout << "Maximum ray tree depth is " << ANSI_BOLD_YELLOW << g_max_depth << ANSI_RESET_STYLE << "." << endl;

  // Create the tracer object.
  switch (g_tracer) {
    
    // Basic recursive ray tracer.
  case WHITTED:
    cout << "Using " << ANSI_BOLD_YELLOW << "Whitted" << ANSI_RESET_STYLE << " ray tracing." << endl;
    tracer = static_cast<Tracer *>(new WhittedTracer(g_max_depth));
    break;

    // Monte Carlo path tracer.
  case MONTE_CARLO:
    cout << "Using " << ANSI_BOLD_YELLOW << "Monte Carlo" << ANSI_RESET_STYLE << " path tracing." << endl;
    tracer = static_cast<Tracer *>(new PathTracer(g_max_depth));
    break;

    // No tracer specified.
  default:
    cerr << "Must specify a ray tracer with \"-t\"." << endl;
    print_usage(argv);
    return EXIT_FAILURE;
  }
  
  // Generate the image.
  // First calculate the number of samples to compute.
  total = static_cast<uint64_t>(g_h) * static_cast<uint64_t>(g_w) * static_cast<uint64_t>(g_samples);
  cout << "Tracing a total of " << ANSI_BOLD_YELLOW << total << ANSI_RESET_STYLE << " primary rays:" << endl;

  // Trace for every row of the image.
  for (int i = 0; i < g_h; i++) {
    // Trace for every column of the image.
    for (int j = 0; j < g_w; j++) {
      // Trace for every sample per pixel.
      for (int k = 0; k < g_samples; k++) {
	// Generate a pixel sample.
	sample = sample_pixel(i, j, g_w, g_h, g_a_ratio, g_fov);
	// Generate a primary ray.
	r = Ray(normalize(vec3(sample, -0.5f) - vec3(0.0f)), // The direction of the ray is towards the sample in the image plane.
		vec3(0.0f)                                   // The origin of the ray is at the center of coordinates.
		);
	// Apply the inverse of the view matrix to take the primary ray from camera space to world space.
	scn->m_cam->view_to_world(r);
	// Trace the ray recursively.
	image[i][j] += tracer->trace_ray(r, scn, 0);
	// Add 1 to samples processed.
	current++;
      }
      // Average the samples for this pixel.
      image[i][j] /= g_samples;
    }
    cout << "\r" << ANSI_BOLD_YELLOW << current << ANSI_RESET_STYLE << " of " << ANSI_BOLD_YELLOW << total << ANSI_RESET_STYLE << " primary rays traced.";
  }
  cout << endl;

  // Copy the pixels to the output bitmap.
  cout << "Saving output image." << endl;
  if (g_tracer == MONTE_CARLO) {
    // Allocate an HDR image.
    input_bitmap = FreeImage_AllocateT(FIT_RGBF, g_w, g_h, 96);

    // Get image stride and pixels.
    pitch = FreeImage_GetPitch(input_bitmap);
    bits = (BYTE *)FreeImage_GetBits(input_bitmap);

    // Fill every pixel of the temporary image with the traced image's.
    for (unsigned int y = 0; y < FreeImage_GetHeight(input_bitmap); y++) {
      pixel = (FIRGBF *)bits;
      for (unsigned int x = 0; x < FreeImage_GetWidth(input_bitmap); x++) {
	pixel[x].red = image[g_h - 1 - y][x].r;
	pixel[x].green = image[g_h - 1 - y][x].g;
	pixel[x].blue = image[g_h - 1 - y][x].b;
      }
      bits += pitch;
    }

    // If the tracer is the monte carlo path tracer, apply a tone mapping operator to reduce image range.
    output_bitmap = FreeImage_ToneMapping(input_bitmap, FITMO_DRAGO03, g_gamma, g_exposure);

    // Save the output image.
    fif = FreeImage_GetFIFFromFilename(g_out_file_name != NULL ? g_out_file_name : OUT_FILE);
    FreeImage_Save(fif, output_bitmap, g_out_file_name != NULL ? g_out_file_name : OUT_FILE);
    
// Release the bitmaps.
    FreeImage_Unload(input_bitmap);
    FreeImage_Unload(output_bitmap);
    
  } else {
    // Allocate an RGB image.
    input_bitmap = FreeImage_Allocate(g_w, g_h, 24, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);

    // Get image stride and pixels.
    pitch = FreeImage_GetLine(input_bitmap) / FreeImage_GetWidth(input_bitmap);
    bits = (BYTE *)FreeImage_GetBits(input_bitmap);

    // Fill every pixel of the temporary image with the traced image's.
    for (unsigned int y = 0; y < FreeImage_GetHeight(input_bitmap); y++) {
      bits = FreeImage_GetScanLine(input_bitmap, y);
      for (unsigned int x = 0; x < FreeImage_GetWidth(input_bitmap); x++) {
	bits[FI_RGBA_RED] = static_cast<BYTE>(image[g_h - 1 - y][x].r * 255.0f);
	bits[FI_RGBA_GREEN] = static_cast<BYTE>(image[g_h - 1 - y][x].g * 255.0f);
	bits[FI_RGBA_BLUE] = static_cast<BYTE>(image[g_h - 1 - y][x].b * 255.0f);
	bits += pitch;
      }
    }

    // Apply gamma correction.
    FreeImage_AdjustGamma(input_bitmap, g_gamma);

    // Save the output image.
    fif = FreeImage_GetFIFFromFilename(g_out_file_name != NULL ? g_out_file_name : OUT_FILE);
    FreeImage_Save(fif, input_bitmap, g_out_file_name != NULL ? g_out_file_name : OUT_FILE);

    // Release the bitmaps.
    FreeImage_Unload(input_bitmap);
  }

  // Clean up.
  if (g_out_file_name != NULL)
    free(g_out_file_name);

  delete scn;
  delete tracer;

  // Close FreeImage.
  FreeImage_DeInitialise();
  
  return EXIT_SUCCESS;
}

////////////////////////////////////////////
// Helper functions.
////////////////////////////////////////////
void print_usage(char ** const argv) {
  cerr << "USAGE: " << argv[0] << " [OPTIONS]... FILE" << endl;
  cerr << "Renders the scene specified by the scene file FILE." << endl << endl;
  cerr << "Mandatory options: " << endl;
  cerr << "  -t\tRay tracing method to use. Valid values: " << endl;
  cerr << "    \t" << ANSI_BOLD_YELLOW << "whitted" << ANSI_RESET_STYLE << "     Classic Whitted ray tracing." << endl;
  cerr << "    \t" << ANSI_BOLD_YELLOW << "monte_carlo" << ANSI_RESET_STYLE << " Monte Carlo path tracing." << endl;
  cerr << "Extra options:" << endl;
  cerr << "  -o\tOutput image file name with extension." << endl;
  cerr << "    \tDefaults to \"output.png\"." << endl;
  cerr << "  -f\tField of view to use in degrees." << endl;
  cerr << "    \tDefaults to 45.0 degrees." << endl;
  cerr << "  -s\tNumber of samples per pixel." << endl;
  cerr << "    \tDefaults to 25 samples." << endl;
  cerr << "  -w\tImage size in pixels as \"WIDTHxHEIGHT\"." << endl;
  cerr << "    \tDefaults to 640x480 pixels." << endl;
  cerr << "    \tMinimum resolution is 1x1 pixels." << endl;
  cerr << "    \tMaxmimum resolution is " << MAX_W << "x" << MAX_H << " pixels." << endl;
  cerr << "  -r\tMaxmimum recursion depth." << endl;
  cerr << "    \tDefaults to 5." << endl;
  cerr << "  -g\tGamma correction value (>= 0)." << endl;
  cerr << "    \tDefaults to 2.2" << endl;
  cerr << "  -e\tExposure scale factor (in [-8, 8])." << endl;
  cerr << "    \tDefaults to 0.0 (no correction)." << endl;
}

void parse_args(int argc, char ** const argv) {
  int opt;
  int x_pos;

  // Check command line arguments.
  if(argc == 1) {
    print_usage(argv);
    exit(EXIT_FAILURE);
  }

  while((opt = getopt(argc, argv, "-:t:s:w:f:o:r:g:e:p:h:k:c:l:m:z:")) != -1) {
    switch (opt) {
    case 1:
      g_input_file = (char *)malloc((strlen(optarg) + 1) * sizeof(char));
      strcpy(g_input_file, optarg);
      break;

    case 'g':
      g_gamma = atof(optarg);
      g_gamma = g_gamma < 0.0f ? 0.0f : g_gamma;
      break;

    case 'e':
      g_exposure = atof(optarg);
      g_exposure = clamp(g_exposure, -8.0f, 8.0f);
      break;
      
    case 't':
      if (strcmp("whitted", optarg) == 0 )
	g_tracer = WHITTED;
      else if(strcmp("monte_carlo", optarg) == 0 || strcmp("montecarlo", optarg) == 0)
	g_tracer = MONTE_CARLO;
      else {
	cerr << "Invalid ray tracer: " << optarg << endl;
	print_usage(argv);
	exit(EXIT_FAILURE);
      }

      break;

    case 'w':
      for (x_pos = 0; optarg[x_pos]; x_pos++)
	if (optarg[x_pos] == 'x')
	  break;

      if (optarg[x_pos] == '\0') {
	cerr << "Invalid screen resolution: " << optarg << endl;
	print_usage(argv);
	exit(EXIT_FAILURE);
      } else {
	optarg[x_pos] = '\0';
	g_w = atoi(optarg);
	g_h = atoi(&optarg[x_pos + 1]);
	if (g_w <= 0 || g_h <= 0 || g_w >= MAX_W || g_h >= MAX_H) {
	  cerr << "Invalid screen resolution: " << optarg << endl;
	  print_usage(argv);
	  exit(EXIT_FAILURE);
	}
	g_a_ratio = static_cast<float>(g_w) / static_cast<float>(g_h);
      }

      break;

    case 's':
      g_samples = atoi(optarg);
      if (g_samples <= 0) {
	cerr << "Samples per pixel must be a positive integer." << endl;
	print_usage(argv);
	exit(EXIT_FAILURE);
      }

      break;

    case 'o':
      g_out_file_name = (char*)malloc((strlen(optarg) + 1) * sizeof(char));
      strcpy(g_out_file_name, optarg);

      break;

    case 'f':
      g_fov = atof(optarg);
      if (g_fov < 1.0f) {
	cerr << "FoV must be greater than or equal to 1.0 degrees." << endl;
	print_usage(argv);
	exit(EXIT_FAILURE);
      }
      
      break;

    case 'r':
      g_max_depth = static_cast<unsigned int>(abs(atoi(optarg)));
      if (g_max_depth == 0) {
	cerr << "Recursion depth must be a positive integer." << endl;
	print_usage(argv);
	exit(EXIT_FAILURE);
      }

      break;
      
    case ':':
      cerr << "Option \"-" << static_cast<char>(optopt) << "\" requires an argument." << endl;
      print_usage(argv);
      exit(EXIT_FAILURE);
      
      break;

    case '?':
    default:
      cerr << "Unrecognized option: \"-" << static_cast<char>(optopt) << "\"." << endl;
    }
  }

  if (g_input_file == NULL) {
    cerr << "Must specify an input file." << endl;
    print_usage(argv);
    exit(EXIT_FAILURE);
  }
}
