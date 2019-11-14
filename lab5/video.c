#include "video.h"

vbe_mode_info_t info;
uint16_t x_res, y_res;
uint8_t bits_per_pixel, bytes_per_pixel, vg_mode;
uint32_t red_mask, green_mask, blue_mask;
uint8_t red_field_position, green_field_position, blue_field_position;
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

	reg.ah = VBE_AH_FUNCTION_INDICATOR;
	reg.al = VBE_AL_SET_MODE;
	// reg.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
	reg.bx = VBE_BX_SET_MODE_ARGUMENT | mode; // set bit 14: linear framebuffer
	reg.intno = 0x10;

	if(sys_int86(&reg) != OK ) {
		printf("set_vbe_mode: sys_int86() failed \n");
		return 1;
	}

	return 0;
}


void* (vg_init)(uint16_t mode) {	

  /* Allow memory mapping */
	//privctl(BASE_PHYS_ADDRESS, MiB);

	if (vbe_get_mode_info(mode, &info))
		return NULL;

	x_res = info.XResolution;
	y_res = info.YResolution;
	vg_mode = info.MemoryModel;

	// If in direct mode, calculate the masks
	if (info.MemoryModel == VG_MODE_DIRECT) {

		red_mask = 0;
		red_field_position = info.RedFieldPosition;
		for (uint32_t i = info.RedFieldPosition; i < info.RedFieldPosition + info.RedMaskSize; ++i)
			red_mask |= BIT(i); 
		
		green_mask = 0;
		green_field_position = info.GreenFieldPosition;
		for (uint32_t i = info.GreenFieldPosition; i < info.GreenFieldPosition + info.GreenMaskSize; ++i)	
			green_mask |= BIT(i); 
		
		blue_mask = 0;
		blue_field_position = info.BlueFieldPosition;
		for (uint32_t i = info.BlueFieldPosition; i < info.BlueFieldPosition + info.BlueMaskSize; ++i)
			blue_mask |= BIT(i); 
	}

	// +7 para garantir q se for necessário reservamos espaço para o final do byte
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


int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
	return 0;	
}


// base_address + (y * x_res + x) * bytes_per_pixel;
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
	// Index mode
	uint8_t *base = (uint8_t *) buffer_base + (y * x_res + x) * bytes_per_pixel;
	for (uint16_t i = 0; i < len; ++i) {
		if (vg_mode == VG_MODE_INDEXED) {
			*base = (uint8_t) color;
			++base;
		}
		else if (vg_mode == VG_MODE_DIRECT) {
			*base = (uint8_t) ((color & red_mask) >> red_field_position);
			++base;
			*base = (uint8_t) ((color & green_mask) >> green_field_position);
			++base;
			*base = (uint8_t) ((color & blue_mask) >> blue_field_position);
			++base;
		}
		else return 1;
	}
	return 0;
}


int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
	//printf("R: %x G: %x B: %x\n", (color & red_mask) >> red_field_position, (color & green_mask) >> green_field_position, (color & blue_mask >> blue_field_position));
	for (uint16_t i = y; i < y + height; ++i) {
		vg_draw_hline(x, i, width, color);
	}
  return 0;
}	
