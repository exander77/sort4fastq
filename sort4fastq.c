#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

extern int errno;

int main(int argc, char *argv[])
{
    FILE *f1 = NULL, *f2 = NULL;
    char *l1 = NULL, *l2 = NULL, *d1 = NULL, *d2 = NULL, *d3 = NULL;
    size_t c1 = 0, c2 = 0;
    size_t s = 0;
    size_t r;
    size_t skip = 0;
    char *ch1, *ch2;

    if (argc < 3) {
      fprintf(stderr, "Usage:       diff4fastq FILE_R1 FILE_R2\n");
      fprintf(stderr, "Example:     zcat FILE_SA_L001_R2_001.fastq.gz > FILE_SA_L001_R2_001.fastq \n");
      fprintf(stderr, "Example:     sort4fastq <(zcat FILE_SA_L001_R1_001.fastq.gz) \\\n");
      fprintf(stderr, "Example:       FILE_SA_L001_R2_001.fastq \\\n");
      fprintf(stderr, "Example:       gzip > FILE_SA_L001_R2_001.fastq.gz\n");
      fprintf(stderr, "Description: Sorts FILE_R2 based on FILE_R1 ordering, FILE_R2 needs to be decompressed.\n");
      exit(EXIT_FAILURE);
    }

    f1 = fopen(argv[1], "r");
    if (!f1) {
      fprintf(stderr, "Error opening file: %s (%d)\n", strerror(errno), errno);
      exit(EXIT_FAILURE);
    }
    
    f2 = fopen(argv[2], "r");
    if (!f2) {
      fprintf(stderr, "Error opening file: %s (%d)\n", strerror(errno), errno);
      exit(EXIT_FAILURE);
    }

    while ((r = getdelim(&l1, &s, '\n', f1)) != -1) {
        ch1 = strrchr(l1, '/');
        if (ch1) *ch1 = '\0';
        else {
          fprintf(stderr, "Not recognized paired-end reads: %s", l1);
          exit(EXIT_FAILURE);
        }
        if (c1 % 10000000 == 0) fprintf(stderr, "T: %d\n", c1);
        if (getline(&d1, &s, f1) == -1 || 
            getline(&d2, &s, f1) == -1 ||
            getline(&d3, &s, f1) == -1) {
          fprintf(stderr, "Expected three lines after ID.");
          exit(EXIT_FAILURE);
        }
        //fprintf(stderr, "L1: %s\n", l1);
        // seeking in second file
      seek:
        while ((r = getdelim(&l2, &s, '\n', f2)) != -1) {
          ch2 = strrchr(l2, '/');
          if (ch2) *ch2 = '\0';
          if (c2 % 10000000 == 0) fprintf(stderr, "P: %d\n", c2);
          if (getline(&d1, &s, f2) == -1 || 
              getline(&d2, &s, f2) == -1 ||
              getline(&d3, &s, f2) == -1) {
            fprintf(stderr, "Expected three lines after ID.");
            exit(EXIT_FAILURE);
          }
          //fprintf(stderr, "L2: %s\n", l2);
          if (!strcmp(l1, l2)) {
            if (ch2) *ch2 = '/';
            if (skip) {
              fprintf(stderr, "S %d\n", skip);
              skip = 0;
              fprintf(stderr, "M %d %d %s\n", c1, c2, l1);
              fflush(stdout);
            }
            //match
            printf("%s", l2);
            printf("%s", d1);
            printf("%s", d2);
            printf("%s", d3);
            ++c2;
            break;
          } else {
            // skip counter
            ++skip;
            ++c2;
          }
        }
        if (r == -1) {
          rewind(f2);
          c2 = 0;
          fprintf(stderr, "R\n");
          goto seek;
        }
        ++c1;
    }
    
    
    if (l1)
        free(l1);
    if (l2)
        free(l2);

    fclose(f1);
    fclose(f2);

    exit(EXIT_SUCCESS);
}
