{
  "targets": [
    {
      "target_name": "unholy_banking",
      "sources": [
        "unholy_banking.cc",
        "ab_gui_interface.cc"
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
