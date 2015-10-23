{
  "targets": [
    {
      "target_name": "unholy_banking",
      "sources": [
        "src/unholy_banking.cc",
        "src/ab_gui_interface.cc"
      ],
      "include_dirs": [
        "gwenhywfar/gwenhywfar4",
        "aqbanking/aqbanking5"
      ],
      "libraries": [
        "-laqbanking",
        '-lgwenhywfar'
      ]
    }
  ]
}
