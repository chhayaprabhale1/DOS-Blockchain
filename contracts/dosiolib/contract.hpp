#pragma once

namespace dosio {

/**
 * @defgroup contracttype Contract Type
 * @ingroup types
 * @brief Defines contract type which is %base class for every DOSIO contract
 * 
 * @{
 * 
 */

/**
 * @brief %Base class for DOSIO contract.
 * @details %Base class for DOSIO contract. %A new contract should derive from this class, so it can make use of DOSIO_ABI macro.
 */
class contract {
   public:
      /**
       * Construct a new contract given the contract name
       * 
       * @brief Construct a new contract object.
       * @param n - The name of this contract
       */
      contract( account_name n ):_self(n){}
      
      /**
       * 
       * Get this contract name
       * 
       * @brief Get this contract name.
       * @return account_name - The name of this contract
       */
      inline account_name get_self()const { return _self; }

   protected:
      /**
       * The name of this contract
       * 
       * @brief The name of this contract.
       */
      account_name _self;
};

/// @} contracttype
} /// namespace dosio
