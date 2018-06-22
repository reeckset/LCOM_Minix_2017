#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macros.h"
#include "vbe.h"
#include "video_gr.h"
#include "video_test.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR	0xE0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8

/* Private global variables */

static char *video_mem;		/* Process (virtual) address to which VRAM is mapped */

static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

unsigned int getHorResolution() {
	return h_res;
}

unsigned int getVerResolution() {
	return v_res;
}

char * getGraphicsBuffer() {
	return video_mem;
}

void* vg_init(unsigned short mode) {

	//SET GRAPHIC MODE
	struct reg86u r;
	r.u.w.ax = VBE_CALL_CMD | VBE_SET_GRAPHIC_MODE; // VBE call, function 02 -- set VBE mode
	r.u.w.bx = LINEAR_FRAMEBUFFER|mode; // set bit 14: linear framebuffer
	r.u.b.intno = VBE_INTERFACE;
	if( sys_int86(&r) != OK ) {
		printf("set_vbe_mode: sys_int86() failed \n");
		return NULL;
	}

	//GET VBE MODE INFO
	vbe_mode_info_t vmi_p;
	vbe_get_mode_info(GRAPHIC_MODE, &vmi_p);
	long phys_address = vmi_p.PhysBasePtr;
	h_res = vmi_p.XResolution;
	v_res = vmi_p.YResolution;
	bits_per_pixel = vmi_p.BitsPerPixel;


	//MAP MEMORY
	int l;
	struct mem_range mr;
	unsigned int vram_base = phys_address; /* VRAM’s physical addresss */
	unsigned int vram_size = h_res * v_res * bits_per_pixel; /* VRAM’s size, but you can use
	the frame-buffer size, instead */
	/* Allow memory mapping */
	mr.mr_base = (phys_bytes) vram_base;
	mr.mr_limit = mr.mr_base + vram_size;
	if( OK != (l = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
	panic("sys_privctl (ADD_MEM) failed: %d\n", l);
	/* Map memory */
	video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
	if(video_mem == MAP_FAILED)
	//panic("couldn’t map video memory");
	return NULL;
	else{
		return video_mem;
	}


}


int vg_exit() {
  struct reg86u reg86;

	reg86.u.b.intno = VBE_INTERFACE;
	reg86.u.b.ah = VBE_SET_TEXT_MODE;
	reg86.u.b.al = TEXT_MODE;

  if( sys_int86(&reg86) != OK ) {
      printf("\tvg_exit(): sys_int86() failed \n");
      return 1;
  } else
      return 0;
}

int video_draw_pixel(unsigned short x, unsigned short y, unsigned long color) {

	//first pixel address : video_mem
	if(x >= 0 && x < h_res && y >= 0 && y < v_res){
		video_mem[h_res*y+x] = (unsigned char) color;
	}else return 1; //Cant draw pixel -> Out of Bounds

	return 0;

}

int clearScreen() {
	int i,j;
	for(i = 0; i < V_RES; i++) {
		for(j = 0; j < H_RES; j++) {
			if(video_draw_pixel(j, i, 0) == 1) return 1;
		}
	}

	return 0;
}


char *read_xpm(char *map[], int *wd, int *ht)
 {
   int width, height, colors;
   char sym[256];
   int  col;
   int i, j;
   char *pix, *pixtmp, *tmp, *line;
   char symbol;

   /* read width, height, colors */
   if (sscanf(map[0],"%d %d %d", &width, &height, &colors) != 3) {
     printf("read_xpm: incorrect width, height, colors\n");
     return NULL;
   }
 #ifdef DEBUG
   printf("%d %d %d\n", width, height, colors);
 #endif
   if (width > H_RES || height > V_RES || colors > 256) {
     printf("read_xpm: incorrect width, height, colors\n");
     return NULL;
   }

   *wd = width;
   *ht = height;

   for (i=0; i<256; i++)
     sym[i] = 0;

   /* read symbols <-> colors */
   for (i=0; i<colors; i++) {
     if (sscanf(map[i+1], "%c %d", &symbol, &col) != 2) {
       printf("read_xpm: incorrect symbol, color at line %d\n", i+1);
       return NULL;
     }
 #ifdef DEBUG
     printf("%c %d\n", symbol, col);
 #endif
     if (col > 256) {
       printf("read_xpm: incorrect color at line %d\n", i+1);
       return NULL;
     }
     sym[col] = symbol;
   }

   /* allocate pixmap memory */
   pix = pixtmp = malloc(width*height);

   /* parse each pixmap symbol line */
   for (i=0; i<height; i++) {
     line = map[colors+1+i];
 #ifdef DEBUG
     printf("\nparsing %s\n", line);
 #endif
     for (j=0; j<width; j++) {
       tmp = memchr(sym, line[j], 256);  /* find color of each symbol */
       if (tmp == NULL) {
           printf("read_xpm: incorrect symbol at line %d, col %d\n", colors+i+1, j);
           return NULL;
       }
       *pixtmp++ = tmp - sym; /* pointer arithmetic! back to books :-) */
 #ifdef DEBUG
       printf("%c:%d ", line[j], tmp-sym);
 #endif
     }
   }

   return pix;
 }

void get_vbe_physical_mem(){

}
