{
  "targets": [
    {
      "target_name": "unholy_banking",
      "sources": [
        "src/unholy_banking.cc",
        "src/ab_gui_interface.cc",
        "src/ab_helpers.cc"
      ],
      "include_dirs": [
        "/usr/include/aqbanking5",
        "/usr/include/gwenhywfar4"
      ],
      "libraries": [
        "-L/usr/lib",
        "-laqbanking",
        "-laqhbci"
      ]
    }
  ]
}
