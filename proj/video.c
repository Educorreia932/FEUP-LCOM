#include "video.h"

vbe_mode_info_summary_t vg_info;

void* buffer_base = NULL;
void* double_buffer_base = NULL;
size_t size;

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
	reg.intno = VBE_INTERRUPT_NUMBER;

	if(sys_int86(&reg) != OK ) {
		printf("vg_set_mode: sys_int86() failed \n");
		return 1;
	}

	if (reg.ah != VBE_FUNCTION_RET_SUCCESS || reg.al != VBE_AH_FUNCTION_INDICATOR) {
		printf("vg_set_mode: function return value different from success: %x\n", reg.al);
		return 1;
	}

	return 0;
}

int vbe_get_mode_info_ours(uint16_t mode, vbe_mode_info_t *vbe_info) {

	privctl(BASE_PHYS_ADDRESS, MiB);

	mmap_t map; 
	if(lm_alloc(sizeof(vbe_mode_info_t), &map) == NULL) {
		printf("Error: failed to allocate memory block.");
		return 1;
	}

	reg86_t reg;
	memset(&reg, 0, sizeof(reg86_t));

	// Set function
	reg.ah = VBE_AH_FUNCTION_INDICATOR;
	reg.al = VBE_AL_MODE_INFO;
	// The struct to save the information
	reg.es = PB2BASE(map.phys);
	reg.di = PB2OFF(map.phys);
	// Set interrupt
	reg.intno = VBE_INTERRUPT_NUMBER;
	// Set mode
	reg.cx = mode;

	if(sys_int86(&reg) != OK ) {
		printf("vbe_get_mode_info_ours: sys_int86() failed \n");
		return 1;
	}

	if (reg.ah != VBE_FUNCTION_RET_SUCCESS || reg.al != VBE_AH_FUNCTION_INDICATOR) {
		printf("vbe_get_mode_info_ours: function return value different from success: %x\n", reg.ax);
		printf("reg.ah: %x reg.ax: %x reg.al: %x\n", reg.ah, reg.ax, reg.al);
		return 1;
	}

	*vbe_info = *((vbe_mode_info_t*) map.virt);

	if (!lm_free(&map)) {
		printf("vbe_get_mode_info_ours: couldn't deallocate memory in the low memory region\n");
		return 1;
	}

	return 0;
}

void* (vg_init)(uint16_t mode) {
	vbe_mode_info_t vbe_info;

	if (vbe_get_mode_info_ours(mode, &vbe_info)) {
		return NULL;
	}

	// According to the VBE standard bit 0 of ModeAttributes set to 0
	// means the mode is not supported by the hardware (1 means supported)
	// this is redundant we hope, but better safe than sorry
	if ((vbe_info.ModeAttributes & BIT(0)) == 0)
		return NULL;

	vg_info.x_res = vbe_info.XResolution;
	vg_info.y_res = vbe_info.YResolution;
	vg_info.vg_mode = vbe_info.MemoryModel;

	// If in direct mode, calculate the masks
	if (vbe_info.MemoryModel == VG_MODE_DIRECT) {

		vg_info.red_mask = 0;
		vg_info.red_field_position = vbe_info.RedFieldPosition;
		vg_info.red_mask_size = vbe_info.RedMaskSize;
		
		for (uint32_t i = vbe_info.RedFieldPosition; i < vbe_info.RedFieldPosition + vbe_info.RedMaskSize; ++i)
			vg_info.red_mask |= BIT(i); 
		
		vg_info.green_mask = 0;
		vg_info.green_field_position = vbe_info.GreenFieldPosition;
		vg_info.green_mask_size = vbe_info.GreenMaskSize;
		
		for (uint32_t i = vbe_info.GreenFieldPosition; i < vbe_info.GreenFieldPosition + vbe_info.GreenMaskSize; ++i)	
			vg_info.green_mask |= BIT(i); 
		
		vg_info.blue_mask = 0;
		vg_info.blue_field_position = vbe_info.BlueFieldPosition;
		vg_info.blue_mask_size = vbe_info.BlueMaskSize;
		
		for (uint32_t i = vbe_info.BlueFieldPosition; i < vbe_info.BlueFieldPosition + vbe_info.BlueMaskSize; ++i)
			vg_info.blue_mask |= BIT(i); 
	}

	// +7 para garantir q se for necessário reservamos espaço para o final do byte
	vg_info.bits_per_pixel = vbe_info.BitsPerPixel;
	vg_info.bytes_per_pixel = (vg_info.bits_per_pixel + 7) >> 3;
	size = vg_info.x_res * vg_info.y_res * vg_info.bytes_per_pixel;

	privctl((phys_bytes) vbe_info.PhysBasePtr, size);

	buffer_base = vm_map_phys(SELF, (void *) vbe_info.PhysBasePtr, size);
	
	if (buffer_base == MAP_FAILED)
		printf("Mapping failed\n");

	if (vg_set_mode(mode))
		return NULL;

	double_buffer_base = malloc(size);

	return buffer_base;
}

void vg_fill_black() {
	memset(buffer_base, 0, vg_info.x_res * vg_info.y_res * vg_info.bytes_per_pixel);
}

void vg_fill_color_rgb_565(uint16_t color) {
	uint16_t* buffer = (uint16_t*) buffer_base;
	for (uint32_t i = 0; i < vg_info.x_res * vg_info.y_res; ++i) {
		*buffer = color;
		++buffer;
	}
}

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {

	uint8_t *base = (uint8_t *) buffer_base + (y * vg_info.x_res + x) * vg_info.bytes_per_pixel;

	for (uint8_t j = 0; j < vg_info.bytes_per_pixel; ++j) {
		*base = color >> (j * 8);
		++base;
	}

	return 0;	
}

// base_address + (y * x_res + x) * bytes_per_pixel;
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {

	// Index mode
	uint8_t *base = (uint8_t *) buffer_base + (y * vg_info.x_res + x) * vg_info.bytes_per_pixel;
	for (uint16_t i = 0; i < len; ++i) {

		for (uint8_t j = 0; j < vg_info.bytes_per_pixel; ++j) {
			*base = color >> (j * 8);
			++base;
		}
		
	}

	return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
	
	if (x > vg_info.x_res || y > vg_info.y_res)
		return 0;

	if (x + width >= vg_info.x_res) {
		width = vg_info.x_res - x;
	}

	if (y + height >= vg_info.y_res) {
		height = vg_info.y_res - y;
	}

	for (uint16_t i = y; i < y + height; ++i) {
		vg_draw_hline(x, i, width, color);
	}

	return 0;

}

void switch_double_buffer() {
	// Copying values, since page flipping isn't possible
	memcpy(buffer_base, double_buffer_base, size);
}
