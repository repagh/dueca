}; // inserted as last thing in the class definition. Close off the class



#define __CUSTOM_COMPATLEVEL_MSGPACK_1
#define __CUSTOM_COMPATLEVEL_MSGPACK_2

/** This ensures that this DCO objects are marked as such when trying
    to pack to msgpack as array */
inline const msgpack_dco_array<PupilRemoteHeadPose>&
mark_for_dco_msgpack(const PupilRemoteHeadPose &obj)
{
  return *reinterpret_cast<const msgpack_dco_array<PupilRemoteHeadPose>*>(&obj);
}


#define __CUSTOM_MSGPACK_PACK_PupilRemoteHeadPose
namespace msgpack {
/// @cond
MSGPACK_API_VERSION_NAMESPACE(v1) {
/// @endcond
namespace adaptor {

#if DUECA_MSGPACK_CODEGEN_VERSION == 2

/// member size for this type of object, including parent members
struct PupilRemoteHeadPose_membersize
{
  /// return number of elements in object
  static constexpr unsigned n_members() {
  return 6U;
  }
};

/// msgpack pack specialization with array coding
template <>
struct pack<msgpack_dco_array<PupilRemoteHeadPose>>:
  public PupilRemoteHeadPose_membersize
{
  // main operator, packs data
  template <typename Stream>
  msgpack::packer<Stream>& operator()(
    msgpack::packer<Stream>& o,
    const msgpack_dco_array<PupilRemoteHeadPose>& v) const
  {
    o.pack_array(this->n_members());
    this->pack_members<Stream>(o, v);
    return o;
  }

  template <typename Stream>
  static void pack_members(msgpack::packer<Stream>& o,
                           const PupilRemoteHeadPose& v)
  {
    o.pack(mark_for_dco_msgpack(v.topic));
    o.pack(mark_for_dco_msgpack(v.camera_extrinsics));
    o.pack(mark_for_dco_msgpack(v.camera_poses));
    o.pack(mark_for_dco_msgpack(v.camera_trace));
    o.pack(*reinterpret_cast<const matrix44double2*>(&v.camera_pose_matrix));
    // o.pack(mark_for_dco_msgpack(v.camera_pose_matrix));
    o.pack(mark_for_dco_msgpack(v.timestamp));
  }

  template<typename S>
  static void unpack_members(S& i0, const S& iend,
                             PupilRemoteHeadPose& v)
  {
    msgunpack::msg_unpack(i0, iend, v.topic);
    msgunpack::msg_unpack(i0, iend, v.camera_extrinsics);
    msgunpack::msg_unpack(i0, iend, v.camera_poses);
    msgunpack::msg_unpack(i0, iend, v.camera_trace);
    msgunpack::msg_unpack(i0, iend, *reinterpret_cast<matrix44double2*>(&(v.camera_pose_matrix)) );
    // msgunpack::msg_unpack(i0, iend, v.camera_pose_matrix);
    msgunpack::msg_unpack(i0, iend, v.timestamp);
   }
};

/// msgpack pack specialization with object coding
template <>
struct pack<PupilRemoteHeadPose>:
  public PupilRemoteHeadPose_membersize
{
  /// main operator, packs DCO as object
  template <typename Stream>
  msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o,
                                      const PupilRemoteHeadPose& v) const
  {
    o.pack_map(this->n_members());
    this->pack_members<Stream>(o, v);
    return o;
  }

  template <typename Stream>
  static void pack_members(msgpack::packer<Stream>& o,
                           const PupilRemoteHeadPose& v)
  {
    o.pack_str(5);
    o.pack_str_body("topic", 5);
    o.pack(v.topic);
    o.pack_str(17);
    o.pack_str_body("camera_extrinsics", 17);
    o.pack(v.camera_extrinsics);
    o.pack_str(12);
    o.pack_str_body("camera_poses", 12);
    o.pack(v.camera_poses);
    o.pack_str(12);
    o.pack_str_body("camera_trace", 12);
    o.pack(v.camera_trace);
    o.pack_str(18);
    o.pack_str_body("camera_pose_matrix", 18);
    o.pack(*reinterpret_cast<const matrix44double2*>(&v.camera_pose_matrix));
    // o.pack(v.camera_pose_matrix);
    o.pack_str(9);
    o.pack_str_body("timestamp", 9);
    o.pack(v.timestamp);
  }
};

#else

/// msgpack pack specialization
template <>
struct pack<PupilRemoteHeadPose> {
  template <typename Stream>
  msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o,
                                      const PupilRemoteHeadPose& v) const
  {
    MSGPACK_DCO_OBJECT(this->n_members());
    this->pack_members<Stream>(o, v);
    return o;
  }
  static constexpr unsigned n_members() {
    return 6U;
  }
  template <typename Stream>
  static void pack_members(msgpack::packer<Stream>& o,
                           const PupilRemoteHeadPose& v)
  {
    MSGPACK_DCO_MEMBER(topic);
    MSGPACK_DCO_MEMBER(camera_poses);
    MSGPACK_DCO_MEMBER(camera_extrinsics);
    MSGPACK_DCO_MEMBER(camera_trace);
    //MSGPACK_DCO_MEMBER(camera_pose_matrix);
    MSGPACK_PACK_MEMBER_ID(o, "camera_pose_matrix");
    o.pack(*reinterpret_cast<const matrix44double2*>(&v.camera_pose_matrix));
    MSGPACK_DCO_MEMBER(timestamp);
  }
};
#endif

} // namespace adaptor
/// @cond
} // MSGPACK_API_VERSION_NAMESPACE(v1)
/// @endcond
} // namespace msgpack


namespace dueca {
namespace messagepack {

#define __CUSTOM_MSGPACK_VISITOR_PupilRemoteHeadPose

/** Gobble visitor, for uncoded "members" */
GobbleVisitor& v_gobble_PupilRemoteHeadPose();

/** Unpackvisitor class for PupilRemoteHeadPose */
template<>
struct UnpackVisitor<msgpack_container_dco,PupilRemoteHeadPose>:
  public DCOUnpackVisitor
{
  /** Reference to the object currently being filled */
  PupilRemoteHeadPose& v;

  /** How many members in the parent */
  static constexpr unsigned parent_n_members = 0U;
  /** How many total? */
  static constexpr unsigned n_members =
    msgpack::v1::adaptor::pack<PupilRemoteHeadPose>::n_members();
  /** Unpackvisitor for member "topic" */
  UnpackVisitor<typename msgpack_visitor<string16>::variant,string16>
    v_topic;
  /** Unpackvisitor for member "camera_poses" */
  UnpackVisitor<typename msgpack_visitor<vector6double>::variant,vector6double>
    v_camera_poses;
  /** Unpackvisitor for member "camera_extrinsics" */
  UnpackVisitor<typename msgpack_visitor<vector6double>::variant,vector6double>
    v_camera_extrinsics;
  /** Unpackvisitor for member "camera_trace" */
  UnpackVisitor<typename msgpack_visitor<vector3double>::variant,vector3double>
    v_camera_trace;
  /** Unpackvisitor for member "camera_pose_matrix" */
  UnpackVisitor<typename msgpack_visitor<matrix44double2>::variant,matrix44double2>
    v_camera_pose_matrix;
  /** Unpackvisitor for member "timestamp" */
  UnpackVisitor<typename msgpack_visitor<double>::variant,double>
    v_timestamp;

  /** Generic reference to the member variable visitors */
  MemberVisitorTable visitors[6];

  /** Constructor */
  UnpackVisitor(PupilRemoteHeadPose &v) :
    DCOUnpackVisitor(),
    v(v),
    v_topic(v.topic),
    v_camera_poses(v.camera_poses),
    v_camera_extrinsics(v.camera_extrinsics),
    v_camera_trace(v.camera_trace),
    v_camera_pose_matrix(*reinterpret_cast<matrix44double2*>(&v.camera_pose_matrix)),
    v_timestamp(v.timestamp),
    visitors{
      { "topic", &v_topic },
      { "camera_poses", &v_camera_poses },
      { "camera_extrinsics", &v_camera_extrinsics },
      { "camera_trace", &v_camera_trace },
      { "camera_pose_matrix", &v_camera_pose_matrix },
      { "timestamp", &v_timestamp }
    }
  { }

  /** Select a specific virtual visitor.

      @param name    If NULL, use the (parent class) variable sel,
                     otherwise determine sel from the name */
  virtual bool setVirtualVisitor(const char* name = NULL, bool isparent=false)
  {
    if (name) {
      sel = 0;
      for (const auto v: visitors) {
        if (!strcmp(v.name, name)) { nest = v.visitor; return true; }
        sel++;
      }
      if (isparent) return false;
      nest = v_gobble_PupilRemoteHeadPose().missingMember(name);
      return true;
    }
    if (sel < int(n_members)) {
      nest = visitors[sel - parent_n_members].visitor;
      return true;
    }
    if (isparent) return false;
    throw msgpack_excess_array_members("PupilRemoteHeadPose");
  }
};

} // namespace messagepack
} // namespace dueca

#if DUECA_MSGPACK_CODEGEN_VERSION == 1
#define __CUSTOM_MSGPACK_UNPACK_PupilRemoteHeadPose
namespace msgunpack {
template<typename S>
void msg_unpack(S& i0, const S& iend, PupilRemoteHeadPose&i)
{
  MSGPACK_CHECK_DCO_SIZE(0);
  MSGPACK_UNPACK_MEMBER(i.topic);
  MSGPACK_UNPACK_MEMBER(i.camera_poses);
  MSGPACK_UNPACK_MEMBER(i.camera_extrinsics);
  MSGPACK_UNPACK_MEMBER(i.camera_trace);
  MSGPACK_UNPACK_MEMBERID(i0, iend, "camera_pose_matrix");
  msg_unpack(i0, iend, *reinterpret_cast<matrix44double2*>(&(i.camera_pose_matrix)) );
  // MSGPACK_UNPACK_MEMBER(i.camera_pose_matrix);
  MSGPACK_UNPACK_MEMBER(i.timestamp);
};
} // namespace msgunpack
#endif



// ensure the } originally closing of class definition is balanced
namespace {
