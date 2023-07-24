#include "CausalModel.h"

namespace gum{
    template<typename GUM_SCALAR>
    INLINE bool CausalModel<GUM_SCALAR>::isIndependent(const std::string&                Xname,
                    const std::string&                Yname,
                    const std::vector< std::string >& Znames) const {
        return isIndependent(idFromName(Xname), idFromName(Yname), nodeset(Znames));
    };

    template<typename GUM_SCALAR>
    INLINE bool CausalModel<GUM_SCALAR>::isIndependent(const std::vector< std::string >& Xnames,
                    const std::vector< std::string >& Ynames,
                    const std::vector< std::string >& Znames) const {
        return isIndependent(nodeset(Xnames), nodeset(Ynames), nodeset(Znames));
    };
}