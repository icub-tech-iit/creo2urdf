/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <creo2urdf/Utils.h>

#include <wfcFeature.h>
#include <wfcElemIds.h>

#include <ProAsmcomp.h>


/**
 * @brief Mapping from ProAsmcompSetType to JointType.
 *
 * This map associates ProAsmcompSetType constants with their corresponding JointType values.
 */
static const std::map<ProAsmcompSetType, JointType> proAsmCompSetType_to_JointType
{
    {PRO_ASM_SET_TYPE_PIN, JointType::Revolute},              ///< Mapping for Pin joint type (1 rotational DOF).
    {PRO_ASM_SET_TYPE_SLIDER, JointType::Linear},             ///< Mapping for Slider joint type (1 translational DOF).
    {PRO_ASM_SET_TYPE_BALL, JointType::Spherical},            ///< Mapping for Ball joint type (3 rotational DOFs).
    {PRO_ASM_SET_TYPE_FIXED, JointType::Fixed},               ///< Mapping for Fixed joint type (zero DOFs).
    {PRO_ASM_SET_TYPE_WELD, JointType::Fixed},                ///< Mapping for Weld joint type (assumed as Fixed).
    {PRO_ASM_SET_USER_DEFINED_TYPE, JointType::Fixed}         ///< Mapping for User-Defined joint type (assumed as Fixed).
};

/**
 * @brief The ElementTreeManager class extracts the ElementTree of a part and provides methods to extract information from it.
 */
class ElementTreeManager {
public:
    /**
     * @brief Default constructor for ElementTreeManager.
     */
    ElementTreeManager();

    /**
     * @brief Constructor for ElementTreeManager with specific parameters.
     * @param feat A pointer to a part casted as feature.
     * @param joint_info_map A map containing joint information.
     */
    ElementTreeManager(pfcFeature_ptr feat, std::map<std::string, JointInfo>& joint_info_map);

    /**
     * @brief Destructor for ElementTreeManager.
     */
    ~ElementTreeManager();

    /**
     * @brief Populates joint information from the given ElementTree.
     * @param feat A pointer to a part casted as feature.
     * @param joint_info_map A map containing joint information.
     * @return True if successful, false otherwise.
     */
    bool populateJointInfoFromElementTree(pfcFeature_ptr feat, std::map<std::string, JointInfo>& joint_info_map);

    /**
     * @brief Gets the constraint type between two assembled parts.
     * @return The constraint type.
     */
    int getConstraintType();

    /**
     * @brief Gets the name of the parent element of the two assembled parts.
     * @return The name of the parent element.
     */
    std::string getParentName();

    /**
     * @brief Gets the name of the child  of the two assembled parts.
     * @return The name of the child element.
     */
    std::string getChildName();

private:
    wfcElementTree_ptr tree = nullptr; ///< Pointer to the ElementTree of the part as feature.
    wfcWFeature_ptr wfeat = nullptr;   ///< Pointer to the part as feature.
    pfcSolid_ptr parent_solid;         ///< Pointer to the parent solid.
    pfcSolid_ptr child_solid;          ///< Pointer to the child solid.

    /**
     * @brief Retrieves the name of a common datum for the given model item type.
     * @param type The model item type.
     * @return The name of the common datum.
     */
    /*std::string retrieveCommonDatumName(pfcModelItemType type);*/

    /**
     * @brief Gets the characterizing constraint datum used for assembling the two parts, depending on the joint.
     * For a revolute joint, the datum is Axis, for a fixed joint the datum is CSys.
     * 
     * @param feat A pointer to a part casted as feature.
     * @param constraint_type The constraint type.
     * @param datum_type The datum type.
     * @return The constraint datum.
     */
    std::string getConstraintDatum(pfcFeature_ptr feat, pfcComponentConstraintType constraint_type, pfcModelItemType datum_type);

    /**
     * @brief Retrieves references to the parent and child solids.
     * @return True if successful, false otherwise.
     */
    bool retrieveSolidReferences();

    /**
     * @brief Retrieves the name of the part associated with the ElementTree.
     * @return The name of the part.
     */
    std::string retrievePartName();

    /**
     * @brief Retrieves the min and max limits for the joint created during assembling the parts.
     * @param feat A pointer to a part casted as feature.
     * @return A pair representing the limits (min, max).
     */
    std::pair<double, double> retrieveLimits(pfcFeature_ptr feat);
};
