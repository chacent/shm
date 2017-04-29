{
  "targets": [
    {
      "target_name": "addon",
      "sources": [ "addon.cc", "shm.cc" ],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
	"include"
      ],
      'libraries':[
      
      ]
    }
  ]
}