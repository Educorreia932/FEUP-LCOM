#include "video.h"

uint16_t x_res, y_res;

void privctl() {
  	int r;
	struct minix_mem_range mr;		
	unsigned int phys_addr = 0;

	mr.mr_base = (phys_bytes) phys_addr;	
	mr.mr_limit = phys_addr + MEM_MAX_SIZE;  

	if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("sys_privctl (ADD_MEM) failed: %d\n", r);
}

// void*
int video_init(uint16_t mode) {	
	vbe_mode_info_t info;

	privctl();

	if (vbe_get_mode_info(&info))
		return 1;

  	/* Allow memory mapping */
	privctl();

    reg86_t reg;

	memset(&reg, 0, sizeof(reg86_t));	

	reg.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
	reg.bx = 1<<14|mode; // set bit 14: linear framebuffer
	reg.intno = 0x10;

	if(sys_int86(&reg) != OK ) {
		printf("set_vbe_mode: sys_int86() failed \n");
		return 1;
	}

	x_res = info.XResolution;
	y_res = info.YResolution;

  	return vm_map_phys(SELF, info.PhysBasePtr, info.x);
}

int vg_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
	
}

int vg_draw_rectangle (uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {

}	
