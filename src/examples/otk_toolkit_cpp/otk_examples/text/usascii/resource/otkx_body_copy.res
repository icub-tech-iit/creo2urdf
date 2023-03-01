/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/

!-------------------------------------------------------------------------------
!
! Reference file for regression test otk_uifc_dialog_p20
!
! MODIFICATIONS:
!
! 10-Dec-19 P-70-37 $$1 rkothari Created
!
!-------------------------------------------------------------------------------
(Dialog otkx_body_copy
    (Components
        (SubLayout Content)
        (Separator CommitSeparator)
        (PushButton CommitOK)
        (PushButton CommitCancel)
    )
    (Resources
        (CommitSeparator.ResourceHints "Template:OKCancelDialog.CommitSeparator")
        (CommitOK.Label "OK")
        (CommitOK.TopOffset 10)
        (CommitOK.BottomOffset 10)
        (CommitOK.LeftOffset 40)
        (CommitOK.RightOffset 0)
        (CommitOK.ResourceHints "Template:OKCancelDialog.CommitOK")
        (CommitCancel.Label "Cancel")
        (CommitCancel.TopOffset 10)
        (CommitCancel.BottomOffset 10)
        (CommitCancel.LeftOffset 8)
        (CommitCancel.RightOffset 10)
        (CommitCancel.ResourceHints "Template:OKCancelDialog.CommitCancel")
        (.TopOffset 0)
        (.BottomOffset 0)
        (.LeftOffset 0)
        (.RightOffset 0)
        (.DefaultButton "CommitOK")
        (.ResourceHints "Template:OKCancelDialog" "Guidelines:Creo4-Dialog" "GuidelinesVersion:2" "Version:Creo4" "@Subgrid0x2.Template:OKCancelDialog.CommitBar")
        (.Accelerator "Esc")
        (.Label "Copy Options")
        (.AttachLeft True)
        (.Layout
            (Grid
                (Rows 1 0 0)
                (Cols 1)
                Content CommitSeparator
                (Grid
                    (Rows 1)
                    (Cols 1 0 0)
                    (Pos 1 2)
                    CommitOK
                    (Pos 1 3)
                    CommitCancel
                )
            )
        )
    )
)

(Layout Content
    (Components
        (CheckButton cp_appear)
        (CheckButton cp_params)
        (CheckButton cp_names)
        (CheckButton cp_layers)
        (CheckButton cp_mats)
        (CheckButton cp_constr)
    )
    (Resources
        (cp_appear.Label "Appearance")
        (cp_appear.Set 1)
        (cp_appear.AttachLeft True)
        (cp_appear.TopOffset 0)
        (cp_appear.BottomOffset 1)
        (cp_appear.LeftOffset 0)
        (cp_appear.RightOffset 0)
        (cp_params.Label "Parameters")
        (cp_params.Set 1)
        (cp_params.AttachLeft True)
        (cp_params.TopOffset 0)
        (cp_params.BottomOffset 1)
        (cp_params.LeftOffset 0)
        (cp_params.RightOffset 0)
        (cp_names.Label "Names")
        (cp_names.Set 1)
        (cp_names.AttachLeft True)
        (cp_names.TopOffset 0)
        (cp_names.BottomOffset 1)
        (cp_names.LeftOffset 0)
        (cp_names.RightOffset 0)
        (cp_layers.Label "Layers")
        (cp_layers.Set 1)
        (cp_layers.AttachLeft True)
        (cp_layers.TopOffset 0)
        (cp_layers.BottomOffset 1)
        (cp_layers.LeftOffset 0)
        (cp_layers.RightOffset 0)
        (cp_mats.Label "Materials")
        (cp_mats.Set 1)
        (cp_mats.AttachLeft True)
        (cp_mats.TopOffset 0)
        (cp_mats.BottomOffset 1)
        (cp_mats.LeftOffset 0)
        (cp_mats.RightOffset 0)
        (cp_constr.Label "Constructions Bodies")
        (cp_constr.Set 1)
        (cp_constr.AttachLeft True)
        (cp_constr.TopOffset 0)
        (cp_constr.BottomOffset 0)
        (cp_constr.LeftOffset 0)
        (cp_constr.RightOffset 0)
        (.AttachLeft True)
        (.AttachRight True)
        (.AttachTop True)
        (.TopOffset 16)
        (.BottomOffset 16)
        (.LeftOffset 10)
        (.RightOffset 10)
        (.ResourceHints "Template:OKCancelDialog.Content")
        (.AttachBottom True)
        (.Layout
            (Grid
                (Rows 0 0 0 0 0 0)
                (Cols 0)
                cp_appear cp_params cp_names cp_layers cp_mats cp_constr
            )
        )
    )
)
