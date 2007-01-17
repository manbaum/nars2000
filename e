D3DRMDEF.H       21,949 Dec/11/99  2:06:30a A    N:\INCLUDE                    
[  473]         D3DRMQUATERNION dqRotateKey;
[  656]  * Returns a unit quaternion that represents a rotation of theta radians
[  660] extern LPD3DRMQUATERNION D3DRMAPI D3DRMQuaternionFromRotation(LPD3DRMQUATERNION quat,
[  667] extern LPD3DRMQUATERNION D3DRMAPI D3DRMQuaternionMultiply(LPD3DRMQUATERNION q,
[  668]                                                           LPD3DRMQUATERNION a,
[  669]                                                           LPD3DRMQUATERNION b);
[  674] extern LPD3DRMQUATERNION D3DRMAPI D3DRMQuaternionSlerp(LPD3DRMQUATERNION q,
[  675]                                                        LPD3DRMQUATERNION a,
[  676]                                                        LPD3DRMQUATERNION b,
[  680]  * Calculate the matrix for the rotation that a unit quaternion represents
[  682] extern void             D3DRMAPI D3DRMMatrixFromQuaternion(D3DRMMATRIX4D dmMat, LPD3DRMQUATERNION lpDqQuat);
[  685]  * Calculate the quaternion that corresponds to a rotation matrix
[  687] extern LPD3DRMQUATERNION D3DRMAPI D3DRMQuaternionFromMatrix(LPD3DRMQUATERNION, D3DRMMATRIX4D);
D3DRMOBJ.H       78,675 Dec/11/99  2:06:34a A    
[  675]     STDMETHOD(SetQuaternion)(THIS_ LPDIRECT3DRMFRAME reference, D3DRMQUATERNION *q) PURE;
[  767]     STDMETHOD(SetQuaternion)(THIS_ LPDIRECT3DRMFRAME3 reference, D3DRMQUATERNION *q) PURE;
[1,411]     STDMETHOD(AddRotateKey)(THIS_ D3DVALUE time, D3DRMQUATERNION *q) PURE;
[1,433]     STDMETHOD(AddRotateKey)(THIS_ D3DVALUE time, D3DRMQUATERNION *q) PURE;
D3DXMATH.H       33,424 Dec/11/99  2:06:56a A    
[  289]     D3DXQUATERNION  operator + () const;
[  290]     D3DXQUATERNION  operator - () const;
[  293]     D3DXQUATERNION operator + ( const D3DXQUATERNION& ) const;
[  294]     D3DXQUATERNION operator - ( const D3DXQUATERNION& ) const;
[  295]     D3DXQUATERNION operator * ( const D3DXQUATERNION& ) const;
[  296]     D3DXQUATERNION operator * ( float ) const;
[  297]     D3DXQUATERNION operator / ( float ) const;
[  299]     friend D3DXQUATERNION operator * (float, const D3DXQUATERNION& );
[  691]     ( D3DXMATRIX *pOut, const D3DXQUATERNION *pQ);
[  703]       const D3DXQUATERNION *pScalingRotation, const D3DXVECTOR3 *pScaling,
[  704]       const D3DXVECTOR3 *pRotationCenter, const D3DXQUATERNION *pRotation,
[  711]       const D3DXQUATERNION *pRotation, const D3DXVECTOR3 *pTranslation);
[  789]     ( const D3DXQUATERNION *pQ );
[  793]     ( const D3DXQUATERNION *pQ );
[  796]     ( const D3DXQUATERNION *pQ1, const D3DXQUATERNION *pQ2 );
[  800]     ( D3DXQUATERNION *pOut );
[  803]     ( const D3DXQUATERNION *pQ );
[  807]     ( D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ );
[  817]     ( const D3DXQUATERNION *pQ, D3DXVECTOR3 *pAxis, float *pAngle );
[  819] // Build a quaternion from a rotation matrix.
[  821]     ( D3DXQUATERNION *pOut, const D3DXMATRIX *pM);
[  825]     ( D3DXQUATERNION *pOut, const D3DXVECTOR3 *pV, float angle );
[  830]     ( D3DXQUATERNION *pOut, float yaw, float pitch, float roll );
[  832] // Quaternion multiplication.  The result represents the rotation Q2 
[  835]     ( D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ1,
[  836]       const D3DXQUATERNION *pQ2 );
[  839]     ( D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ );
[  843]     ( D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ );
[  848]     ( D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ );
[  853]     ( D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ );
[  858]     ( D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ1,
[  859]       const D3DXQUATERNION *pQ2, float t );
[  864]     ( D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ1,
[  865]       const D3DXQUATERNION *pQ2, const D3DXQUATERNION *pQ3,
[  866]       const D3DXQUATERNION *pQ4, float t );
[  870]     ( D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ1,
[  871]       const D3DXQUATERNION *pQ2, const D3DXQUATERNION *pQ3,
     3 files     0 dirs       134,048 dat        262,656 dsk  1,023,932,928 avl
