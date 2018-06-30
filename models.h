
struct UniformModel {
  int min;
  int max;
  int n;
  int get_position(int i ) {

    if(i > max) return (n-1);
    if(i < min) return 0;

    float f_min = (float)min;
    float f_max = (float)max;
    float f_n = (float)n;
    float f_i = (float)i;
    //printf("Get position %i %i %i %i res %f \n", i, min, max, n, (f_i - f_min)/(f_max - f_min) * f_n);
    return (int)((f_i - f_min)/(f_max-f_min) * f_n + 0.5);
  }
};


