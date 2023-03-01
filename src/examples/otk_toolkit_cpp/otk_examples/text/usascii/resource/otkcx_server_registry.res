/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/

!-------------------------------------------------------------------------------
!
! Reference file for regression test otk_uifc_dialog_p20
!
! MODIFICATIONS:
!
! 04-June-14 P-20-54 $$1 rkothari Created
!
!-------------------------------------------------------------------------------

(Dialog otkcx_server_registry
    (Components
        (Tab                            server_cache_tab
                                        server_lay
                                        cache_lay)
        (PushButton                     close_pb)
    )

    (Resources
        (server_cache_tab.Decorated     True)
        (server_cache_tab.TopOffset     5)
        (server_cache_tab.BottomOffset  5)
        (server_cache_tab.LeftOffset    5)
        (server_cache_tab.RightOffset   5)
        (close_pb.Label                 "&Close")
        (close_pb.TopOffset             10)
        (close_pb.BottomOffset          10)
        (.Label                         "Server Registry")
        (.Rows                          32)
        (.Columns                       38)
        (.Focus                         "server_table")
        (.BackgroundColor               3)
        (.CloseButton                   False)
        (.Layout
            (Grid (Rows 1 1) (Cols 1)
                server_cache_tab
                close_pb
            )
        )
    )
)

(Layout server_lay
    (Components
        (SubLayout                      servers_lay)
        (SubLayout                      workspace_lay)
        (SubLayout                      description_lay)
    )

    (Resources
        (.Label                         "Servers")
        (.Decorated                     True)
        (.Layout
            (Grid (Rows 1 1 1) (Cols 1)
                servers_lay
                workspace_lay
                description_lay
            )
        )
    )
)

(Layout servers_lay
    (Components
        (Table                          server_table)
    )

    (Resources
        (server_table.FontStyle         8)
        (server_table.FontSize          10.000000)
        (server_table.AutoHighlight     True)
        (server_table.ColumnNames       "image"
                                        "server"
                                        "status")
        (server_table.ColumnLabels      ""
                                        "Server"
                                        "Status")
        (server_table.ColumnWidths      2
                                        16
                                        8)
        (server_table.ShowGrid          True)
        (server_table.RowSelectionPolicy 4)
        (.Label                         "Servers")
        (.Decorated                     True)
        (.AttachLeft                    True)
        (.AttachRight                   True)
        (.AttachTop                     True)
        (.AttachBottom                  True)
        (.TopOffset                     10)
        (.BottomOffset                  5)
        (.LeftOffset                    5)
        (.RightOffset                   5)
        (.Layout
            (Grid (Rows 1) (Cols 1)
                server_table
            )
        )
    )
)


(Layout workspace_lay
    (Components
        (Table                          workspace_table)
    )

    (Resources
        (workspace_table.ColumnNames    "workspace"
                                        "context")
        (workspace_table.ColumnLabels   "Workspace"
                                        "Context")
        (workspace_table.ColumnWidths   20
                                        16)
        (workspace_table.ShowGrid       True)
        (.Label                         "Server Active Workspace")
        (.Decorated                     True)
        (.AttachLeft                    True)
        (.AttachRight                   True)
        (.AttachTop                     True)
        (.AttachBottom                  True)
        (.TopOffset                     10)
        (.BottomOffset                  5)
        (.LeftOffset                    5)
        (.RightOffset                   5)
        (.Layout
            (Grid (Rows 1) (Cols 1)
                workspace_table
            )
        )
    )
)


(Layout description_lay
    (Components
        (Label                          server_name_lb)
        (Label                          location_lb)
        (Label                          workspace_lb)
    )

    (Resources
        (server_name_lb.Label           "Server Name :")
        (server_name_lb.Columns         32)
        (server_name_lb.AttachLeft      True)
        (server_name_lb.Alignment       0)
        (location_lb.Label              "Location :")
        (location_lb.Columns            32)
        (location_lb.AttachLeft         True)
        (location_lb.Alignment          0)
        (workspace_lb.Label             "Workspace Name :")
        (workspace_lb.Columns           32)
        (workspace_lb.AttachLeft        True)
        (workspace_lb.Alignment         0)
        (.Label                         "Description")
        (.Decorated                     True)
        (.AttachLeft                    True)
        (.AttachRight                   True)
        (.AttachTop                     True)
        (.AttachBottom                  True)
        (.TopOffset                     10)
        (.BottomOffset                  5)
        (.LeftOffset                    5)
        (.RightOffset                   5)
        (.Layout
            (Grid (Rows 1 1 1) (Cols 1)
                server_name_lb
                location_lb
                workspace_lb
            )
        )
    )
)

(Layout cache_lay
    (Components
        (SubLayout                      cache_info_lay)
    )

    (Resources
        (.Label                         "Cache")
        (.Decorated                     True)
        (.Layout
            (Grid (Rows 1) (Cols 1)
                cache_info_lay
            )
        )
    )
)

(Layout cache_info_lay
    (Components
        (Label                          location_label)
        (Label                          ws_space_label)
        (Label                          total_cache_space)
        (Label                          disk_space_label)
        (InputPanel                     ws_space_ip)
        (InputPanel                     total_cache_ip)
        (InputPanel                     disk_space_ip)
        (Label                          label_mb_1)
        (Label                          label_mb_2)
        (Label                          label_mb_3)
        (PushButton                     cache_tools_pb)
        (PushButton                     clear_cache_pb)
    )

    (Resources
        (location_label.Label           "Location :")
        (location_label.AttachLeft      True)
        (location_label.AttachRight     True)
        (location_label.AttachTop       True)
        (location_label.Alignment       0)
        (location_label.FontStyle       8)
        (ws_space_label.Label           "Workspace Cache Size :")
        (ws_space_label.AttachRight     True)
        (ws_space_label.RightOffset     5)
        (total_cache_space.Label        "Total Cache Size :")
        (total_cache_space.AttachRight  True)
        (total_cache_space.RightOffset  5)
        (disk_space_label.Label         "Total Disk Space :")
        (disk_space_label.AttachRight   True)
        (disk_space_label.RightOffset   5)
        (ws_space_ip.Columns            4)
        (ws_space_ip.InputType          2)
        (total_cache_ip.Columns         2)
        (total_cache_ip.MinColumns      2)
        (total_cache_ip.InputType       2)
        (disk_space_ip.Columns          4)
        (disk_space_ip.InputType        2)
        (label_mb_1.Label               "MB")
        (label_mb_1.Columns             4)
        (label_mb_1.AttachLeft          True)
        (label_mb_1.LeftOffset          2)
        (label_mb_2.Label               "MB")
        (label_mb_2.Columns             4)
        (label_mb_2.AttachLeft          True)
        (label_mb_2.LeftOffset          2)
        (label_mb_3.Label               "MB")
        (label_mb_3.Columns             4)
        (label_mb_3.AttachLeft          True)
        (label_mb_3.LeftOffset          2)
        (cache_tools_pb.Label           "&Cache Tools")
        (clear_cache_pb.Label           "&Clear")
        (clear_cache_pb.AttachLeft      True)
        (.AttachLeft                    True)
        (.AttachRight                   True)
        (.AttachTop                     True)
        (.TopOffset                     10)
        (.LeftOffset                    5)
        (.RightOffset                   5)
        (.Layout
            (Grid (Rows 1 1) (Cols 1 1)
                location_label
                (Pos 2 1)
                (Grid (Rows 1 1 0 0 0 0) (Cols 1 1 0)
                    (Pos 3 1)
                    ws_space_label
                    ws_space_ip
                    label_mb_2
                    total_cache_space
                    total_cache_ip
                    label_mb_3
                    disk_space_label
                    disk_space_ip
                    label_mb_1
                    (Pos 6 3)
                    cache_tools_pb
                )
                (Grid (Rows 1 1 1) (Cols 1)
                    (Pos 2 1)
                    clear_cache_pb
                )
            )
        )
    )
)
