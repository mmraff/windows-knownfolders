{
  "targets": [
    {
      "target_name": "winknownfolders",
      "sources": [
        "src/kfolders.cc",
        "src/constants.cc",
        "src/argsresolv.cc",
        "src/kfargsres.cc",
        "src/kferrs.cc",
        "src/wstrutils.cc",
        "src/getidname.cc",
        "src/getid.cc",
        "src/getidlist.cc",
        "src/kflist.cc",
        "src/kfprop.cc",
        "src/retvals.cc",
        "src/pathwrap.cc",
        "src/idlistwrap.cc",
        "src/listwrap.cc",
        "src/namewrap.cc",
        "src/bynamewrap.cc",
        "src/bypathwrap.cc",
        "src/getdefwrap.cc",
        "src/numberwrap.cc",
        "src/module.cc"
      ],
      "include_dirs": [ "<!(node -e \"require('nan')\")" ],
      'msvs_settings': {
        'VCCLCompilerTool': {
          'AdditionalOptions': [ '/EHsc'] 
        }
      }
    }
  ]
}
