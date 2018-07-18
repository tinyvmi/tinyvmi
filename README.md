
# TinyVMI

---

+ Documentation: https://tinyvmi.github.io 
+ GSoC Blog: https://tinyvmi.github.io/gsoc-blog 

---

Prerequisites:

(TODO: fixme).

For libjson-c:
   
+ libtool>=2.2.6b
+ autoconf>=2.64
+ automake>=1.13

## Quick Start

1, compile and install Xen from given source code of xen-4.10.0, with XSM enabled. (tested on Ubuntu 16.04 as dom0, other Linux dist should be fine). 
    Note: 
    -- FLASK policy definition can be found in ./tools/flask/policy/policy/modules/xen/xen.te [fixme]
    -- How to enable XSM and install policy: https://wiki.xenproject.org/wiki/Xen_Security_Modules_:_XSM-FLASK
    
    
2, install target guest VM on Xen.
    Note:
    -- Both target guest vm and TinyVMI are set with FLASK label: 
        seclabel='system_u:system_r:domU_t'


3, [optional] compile and install [LibVMI](https://github.com/libvmi/libvmi), for comparison with TinyVMI.

4, compile and run tinyvmi as follows:

    # cd ./stubdom/tinyvmi
    # bash run.tiny.sh buildrun
    
   Then you'll get outputs like:
   
        Xen Minimal OS!
          start_info: 0xf6000(VA)
            nr_pages: 0x2000
          shared_inf: 0xa0278000(MA)
             pt_base: 0xf9000(VA)
        nr_pt_frames: 0x5
            mfn_list: 0xe6000(VA)
           mod_start: 0x0(VA)
             mod_len: 0
               flags: 0x0
            cmd_line: 
          stack:      0xa4a20-0xc4a20
        MM: Init
              _text: 0x0(VA)
             _etext: 0x745d4(VA)
           _erodata: 0x8d000(VA)
             _edata: 0x8eba0(VA)
        stack start: 0xa4a20(VA)
               _end: 0xe5328(VA)
          start_pfn: 101
            max_pfn: 2000
        
        .... (more)
        
    
