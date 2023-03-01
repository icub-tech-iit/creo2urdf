/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/

!-------------------------------------------------------------------------------
!
! Reference file for  otk app: Naked Window Create
!
! MODIFICATIONS:
!
! 08-Oct-21  P-90-29  $$1  sbinawad  Created
!
!-------------------------------------------------------------------------------
(Dialog emptyDialog
    (Components
        (SubLayout Content)
        (Separator CommitSeparator)
        (PushButton myOk)
        (PushButton myCancel)
    )
    (Resources
        (CommitSeparator.ResourceHints "Template:OKCancelDialog.CommitSeparator")
        (CommitSeparator.Visible False)
        (myOk.Label "OK")
        (myOk.TopOffset 10)
        (myOk.BottomOffset 10)
        (myOk.LeftOffset 40)
        (myOk.RightOffset 0)
        (myOk.ResourceHints "Template:OKCancelDialog.CommitOK")
        (myCancel.Label "Cancel")
        (myCancel.TopOffset 10)
        (myCancel.BottomOffset 10)
        (myCancel.LeftOffset 8)
        (myCancel.RightOffset 10)
        (myCancel.ResourceHints "Template:OKCancelDialog.CommitCancel")
        (.TopOffset 0)
        (.BottomOffset 0)
        (.LeftOffset 0)
        (.RightOffset 0)
        (.DefaultButton "myOk")
        (.ResourceHints "Template:OKCancelDialog" "Guidelines:Creo4-Dialog" "GuidelinesVersion:2" "Version:Creo4" "@Subgrid0x2.Template:OKCancelDialog.CommitBar")
        (.Accelerator "Esc")
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
                    myOk
                    (Pos 1 3)
                    myCancel
                )
            )
        )
    )
)
(Layout Content
    (Components
        (NakedWindow NakedWindow1)
    )
    (Resources
        (NakedWindow1.AttachLeft True)
        (NakedWindow1.AttachTop True)
        (NakedWindow1.ResourceHints "UserOverrideAttrs:AttachRight,AttachBottom")
        (NakedWindow1.TopOffset 0)
        (NakedWindow1.BottomOffset 0)
        (NakedWindow1.LeftOffset 0)
        (NakedWindow1.RightOffset 0)
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
                (Rows 1)
                (Cols 1)
                NakedWindow1
            )
        )
    )
)
