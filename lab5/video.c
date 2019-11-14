#include "video.h"

vbe_mode_info_t info;
uint16_t x_res, y_res;
uint8_t bits_per_pixel, bytes_per_pixel;
void* buffer_base = NULL;

void privctl(phys_bytes mr_base, size_t size) {
  int r;
	struct minix_mem_range mr;

	mr.mr_base = mr_base;	
	mr.mr_limit = mr_base + size;  

	if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("sys_privctl (ADD_MEM) failed: %d\n", r);
}

int vg_set_mode(uint16_t mode) {
  reg86_t reg;
	memset(&reg, 0, sizeof(reg86_t));	

	reg.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
	reg.bx = 1<<14|mode; // set bit 14: linear framebuffer
	reg.intno = 0x10;

	if(sys_int86(&reg) != OK ) {
		printf("set_vbe_mode: sys_int86() failed \n");
		return 1;
	}

	return 0;
}

void* (vg_init)(uint16_t mode) {	

  /* Allow memory mapping */
	privctl(0, BIT(20));

	if (vbe_get_mode_info(mode, &info))
		return NULL;

	// printf("%x\n", info.MemoryModel);
	// Memory mode 4 -> Indexed
	// Memory mode 6 -> Direct color

	// +7 para garantir q se for necessário reservamos espaço para o final do byte
	x_res = info.XResolution;
	y_res = info.YResolution;
	bits_per_pixel = info.BitsPerPixel;
	bytes_per_pixel = (bits_per_pixel + 7) >> 3;
  size_t size = x_res * y_res * bytes_per_pixel;

	privctl((phys_bytes) info.PhysBasePtr, size);
	buffer_base = vm_map_phys(SELF, (void *) info.PhysBasePtr, size);
	
	if (buffer_base == MAP_FAILED)
		printf("Mapping failed\n");

	if (vg_set_mode(mode))
		return NULL;

	return buffer_base;
}

// base_address + (y * x_res + x) * bytes_per_pixel;
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
	// Index mode
	uint8_t *base = (uint8_t *) buffer_base + (y * x_res + x) * bytes_per_pixel;
	for (int i = x; i < len; ++i) {
		*base = (uint8_t) color;
		++base;
	}
	return 1;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
	for (int i = 0; i < height; ++i) {
		if (vg_draw_hline(x, y+i, width, color))
			return 1;
	}
  return 0;
}	
