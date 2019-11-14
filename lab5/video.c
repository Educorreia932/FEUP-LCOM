#include "video.h"

vbe_mode_info_summary_t info;
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

	vbe_mode_info_t vbe_info;
	if (vbe_get_mode_info(mode, &vbe_info)) {
		return NULL;
	}

	// According to the VBE standard bit 0 of ModeAttributes set to 0
	// means the mode is not supported by the hardware (1 means supported)
	// this is redundant we hope, but better safe than sorry
	if ((vbe_info.ModeAttributes & BIT(0)) == 0)
		return NULL;

	info.x_res = vbe_info.XResolution;
	info.y_res = vbe_info.YResolution;
	info.vg_mode = vbe_info.MemoryModel;

	// If in direct mode, calculate the masks
	if (vbe_info.MemoryModel == VG_MODE_DIRECT) {

		info.red_mask = 0;
		info.red_field_position = vbe_info.RedFieldPosition;
		info.red_mask_size = vbe_info.RedMaskSize;
		for (uint32_t i = vbe_info.RedFieldPosition; i < vbe_info.RedFieldPosition + vbe_info.RedMaskSize; ++i)
			info.red_mask |= BIT(i); 
		
		info.green_mask = 0;
		info.green_field_position = vbe_info.GreenFieldPosition;
		info.green_mask_size = vbe_info.GreenMaskSize;
		for (uint32_t i = vbe_info.GreenFieldPosition; i < vbe_info.GreenFieldPosition + vbe_info.GreenMaskSize; ++i)	
			info.green_mask |= BIT(i); 
		
		info.blue_mask = 0;
		info.blue_field_position = vbe_info.BlueFieldPosition;
		info.blue_mask_size = vbe_info.BlueMaskSize;
		for (uint32_t i = vbe_info.BlueFieldPosition; i < vbe_info.BlueFieldPosition + vbe_info.BlueMaskSize; ++i)
			info.blue_mask |= BIT(i); 
	}

	// +7 para garantir q se for necessário reservamos espaço para o final do byte
	info.bits_per_pixel = vbe_info.BitsPerPixel;
	info.bytes_per_pixel = (info.bits_per_pixel + 7) >> 3;
  size_t size = info.x_res * info.y_res * info.bytes_per_pixel;

	privctl((phys_bytes) vbe_info.PhysBasePtr, size);
	buffer_base = vm_map_phys(SELF, (void *) vbe_info.PhysBasePtr, size);
	
	if (buffer_base == MAP_FAILED)
		printf("Mapping failed\n");

	if (vg_set_mode(mode))
		return NULL;

	return buffer_base;
}


int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
	uint8_t *base = (uint8_t *) buffer_base + (y * info.x_res + x) * info.bytes_per_pixel;
	if (info.vg_mode == VG_MODE_INDEXED) {
			*base = (uint8_t) color;
			++base;
		}
	else if (info.vg_mode == VG_MODE_DIRECT) {

		uint32_t coded_color = 0;
		coded_color |= ((color & info.blue_mask) >> info.blue_field_position);
		coded_color |= ((color & info.green_mask) >> info.green_field_position) << (info.blue_mask_size);
		coded_color |= ((color & info.red_mask) >> info.red_field_position) << (info.green_mask_size + info.blue_mask_size);

		uint32_t final_mask = 0xFF;
		for (uint8_t j = 0; j < info.bytes_per_pixel; ++j) {
			*base = (uint8_t) ((coded_color & final_mask) >> (j * 8));
			final_mask <<= 8;
			++base;
		}
	}
	else
		return 1;
	return 0;	
}


// base_address + (y * x_res + x) * bytes_per_pixel;
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
	// Index mode
	uint8_t *base = (uint8_t *) buffer_base + (y * info.x_res + x) * info.bytes_per_pixel;
	for (uint16_t i = 0; i < len; ++i) {
		if (info.vg_mode == VG_MODE_INDEXED) {
			*base = (uint8_t) color;
			++base;
		}
		else if (info.vg_mode == VG_MODE_DIRECT) {

			uint32_t coded_color = 0;
			coded_color |= ((color & info.blue_mask) >> info.blue_field_position);
			coded_color |= ((color & info.green_mask) >> info.green_field_position) << (info.blue_mask_size);
			coded_color |= ((color & info.red_mask) >> info.red_field_position) << (info.green_mask_size + info.blue_mask_size);

			uint32_t final_mask = 0xFF;
			for (uint8_t j = 0; j < info.bytes_per_pixel; ++j) {
				*base = (uint8_t) ((coded_color & final_mask) >> (j * 8));
				final_mask <<= 8;
				++base;
			}
		}
		else return 1;
	}
	return 0;
}


int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
	//printf("Resolution: %xx%x\n", info.x_res, info.y_res);
	//printf("R: %x G: %x B: %x\n", (color & info.red_mask) >> info.red_field_position, (color & info.green_mask) >> info.green_field_position, (color & info.blue_mask >> info.blue_field_position));
	for (uint16_t i = y; i < y + height; ++i) {
		vg_draw_hline(x, i, width, color);
	}
  return 0;
}	
