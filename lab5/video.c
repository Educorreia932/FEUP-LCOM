#include "video.h"

void privctl() {
  int r;
	struct minix_mem_range mr;		
	unsigned int phys_addr = 0;

	mr.mr_base = (phys_bytes) phys_addr;	
	mr.mr_limit = phys_addr + MEM_MAX_SIZE;  

	if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("sys_privctl (ADD_MEM) failed: %d\n", r);
}

int video_init(uint16_t mode) {
  /* Allow memory mapping */
	privctl();

  reg86_t reg;

	memset(&reg, 0, sizeof(reg86_t));	

	reg.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
	reg.bx = 1<<14|mode; // set bit 14: linear framebuffer
	reg.intno = 0x10;

	if( sys_int86(&reg) != OK ) {
		printf("set_vbe_mode: sys_int86() failed \n");
		return 1;
	}

  return 0;
}
