

#ifndef PEER_MANAGER_TYPES_H__
#define PEER_MANAGER_TYPES_H__

#include <stdint.h>
#include <stdbool.h>


/**
 * @file peer_manager_types.h
 *
 * @addtogroup peer_manager
 * @{
 */


#include <stdint.h>
#include <stddef.h>
#include "ble_gap.h"
#include "ble_hci.h"
#include "ble_gatt_db.h"
#include "nrf.h"


/**@brief Handle to uniquely identify a peer for which we have persistently stored data.
 */
typedef uint16_t pm_peer_id_t;

#define PM_PEER_ID_INVALID              0xFFFF /**< Invalid value for @ref pm_peer_id_t. */
#define PM_PEER_ID_N_AVAILABLE_IDS      256    /**< The number of available peer IDs. */
#define PM_LOCAL_DB_LEN_OVERHEAD_BYTES  offsetof(pm_peer_data_local_gatt_db_flash_t, p_data)
#define PM_REMOTE_DB_LEN_OVERHEAD_BYTES offsetof(pm_peer_data_remote_gatt_db_flash_t, p_data)

static __INLINE uint16_t PM_N_WORDS(uint16_t n_bytes)
{
    return ((n_bytes + 3) >> 2);
}



#define PM_SEC_ERROR_BASE 0x0100  /**< The base for Peer Manager defined errors. See @ref PM_SEC_ERRORS and @ref pm_sec_error_code_t. */


/**@defgroup PM_SEC_ERRORS Peer Manager defined security errors.
 * @{ */
#define PM_SEC_ERROR_PIN_OR_KEY_MISSING (PM_SEC_ERROR_BASE + 0x06) /**< Encryption failed because the peripheral has lost the LTK for this bond. See also @ref BLE_HCI_STATUS_CODE_PIN_OR_KEY_MISSING and Table 3.7 ("Pairing Failed Reason Codes") in the Bluetooth Core Specification 4.2, section 3.H.3.5.5.  */
#define PM_SEC_ERROR_MIC_FAILURE        (PM_SEC_ERROR_BASE + 0x3D) /**< Encryption ended with disconnection because of mismatching keys or a stray packet during a procedure. See SoftDevice GAP Message Sequence Charts on encryption, Bluetooth Core Specification 4.2, sections 6.B.5.1.3.1 and 3.H.3.5.5, and/or @ref BLE_HCI_CONN_TERMINATED_DUE_TO_MIC_FAILURE. */
#define PM_SEC_ERROR_DISCONNECT         (PM_SEC_ERROR_BASE + 0xF0) /**< Pairing or encryption did not finish before link disconnected for unrelated reason. */
#define PM_SEC_ERROR_SMP_TIMEOUT        (PM_SEC_ERROR_BASE + 0xF1) /**< Pairing/bonding could not start because an SMP timeout has already happened on this link. This means that no more pairing or bonding can happen on this link. To be able to pair or bond, the link must be disconnected and then reconnected. See Bluetooth specification 4.2 section 3.H.3.4 */
 /** @} */


/**@brief Errors from security procedures in Peer Manager.
 *
 * @details Possible values are defined in @ref PM_SEC_ERRORS and @ref BLE_GAP_SEC_STATUS.
 */
typedef uint16_t pm_sec_error_code_t;


/**@brief Enumeration describing the different procedures that can lead to an encrypted link.
 */
typedef enum
{
    PM_LINK_SECURED_PROCEDURE_ENCRYPTION, /**< Using an LTK shared during a previous bonding procedure to encrypt the link. */
    PM_LINK_SECURED_PROCEDURE_BONDING,    /**< A pairing procedure, followed by a bonding procedure. */
    PM_LINK_SECURED_PROCEDURE_PAIRING,    /**< A pairing procedure with no bonding. */
} pm_sec_procedure_t;


/**@brief Data associated with a bond to a peer.
 */
typedef struct
{
    uint8_t           own_role; /**< The role of the local device during bonding. */
    ble_gap_id_key_t  peer_id;  /**< The peer's peer address and identity resolution key. */
    ble_gap_enc_key_t peer_ltk; /**< The peer's long term encryption key. */
    ble_gap_enc_key_t own_ltk;  /**< Locally generated long term encryption key, distributed to the peer. */
} pm_peer_data_bonding_t;


/**@brief Function for calculating the flash size of bonding data.
 *
 * @return The number of words the data will take in flash.
 */
static __INLINE uint16_t PM_BONDING_DATA_N_WORDS(void)
{
    return PM_N_WORDS(sizeof(pm_peer_data_bonding_t));
}


/**@brief Function for calculating the flash size of service changed pending state.
 *
 * @return The number of words the data will take in flash.
 */
static __INLINE uint16_t PM_SC_STATE_N_WORDS(void)
{
    return PM_N_WORDS(sizeof(bool));
}


/**@brief Data on a local GATT database.
 */
typedef struct
{
    uint32_t  flags;   /**< Flags describing the database attributes. */
    uint16_t  len;     /**< Size of attribute array. */
    uint8_t * p_data;  /**< Array to hold the database attributes. */
} pm_peer_data_local_gatt_db_t;


/**@brief Data on a local GATT database, as formatted in flash.
 */
typedef struct
{
    uint32_t flags;
    uint16_t len;
    uint16_t _padding;
    uint8_t  p_data[];
} pm_peer_data_local_gatt_db_flash_t;


/**@brief Function for calculating the flash size of local GATT database data.
 *
 * @param[in]  local_db_len  The length of the database as reported by the SoftDevice.
 *
 * @return The number of words the data will take in flash.
 */
static __INLINE uint16_t PM_LOCAL_DB_N_WORDS(uint16_t local_db_len)
{
    return PM_N_WORDS(local_db_len + PM_LOCAL_DB_LEN_OVERHEAD_BYTES);
}


/**@brief Function for calculating the length of a local GATT database attribute array.
 *
 * @param[in]  n_words  The number of words the data takes in flash.
 *
 * @return The length of the database attribute array.
 */
static __INLINE uint16_t PM_LOCAL_DB_LEN(uint16_t n_words)
{
    return ((n_words * 4) - PM_LOCAL_DB_LEN_OVERHEAD_BYTES);
}


/**@brief Data on a remote GATT database.
 */
typedef struct
{
    uint32_t            service_count; /**< Number of stored services. */
    ble_gatt_db_srv_t * p_data;        /**< Array to hold the database attributes. */
} pm_peer_data_remote_gatt_db_t;


/**@brief Data on a remote GATT database, as formatted in flash.
 */
typedef struct
{
    uint32_t          service_count;
    ble_gatt_db_srv_t p_data[];
} pm_peer_data_remote_gatt_db_flash_t;



/**@brief Function for calculating the flash size of remote GATT database data.
 *
 * @param[in]  service_count  The number of services in the service array.
 *
 * @return The number of words the data will take in flash.
 */
static __INLINE uint16_t PM_REMOTE_DB_N_WORDS(uint16_t service_count)
{
    return PM_N_WORDS((sizeof(ble_gatt_db_srv_t) * service_count) + PM_REMOTE_DB_LEN_OVERHEAD_BYTES);
}


/**@brief Union of all data associated with one peer.
 */
typedef union
{
    pm_peer_data_bonding_t        * p_bonding_data;            /**< The exchanged bond information in addition to metadata of the bonding. */
    bool                          * p_service_changed_pending; /**< Whether a service changed indication should be sent to the peer. */
    pm_peer_data_local_gatt_db_t  * p_local_gatt_db;           /**< Persistent information pertaining to a peer GATT client. */
    pm_peer_data_remote_gatt_db_t * p_remote_gatt_db;          /**< Persistent information pertaining to a peer GATT server. */
    uint8_t                       * p_application_data;        /**< Arbitrary data to associate with the peer. This data can be freely used by the application. */
} pm_peer_data_unit_t;


/**@brief Immutable version of @ref pm_peer_data_unit_t.
 */
typedef union
{
    pm_peer_data_bonding_t        const * p_bonding_data;
    bool                          const * p_service_changed_pending;
    pm_peer_data_local_gatt_db_t  const * p_local_gatt_db;
    pm_peer_data_remote_gatt_db_t const * p_remote_gatt_db;
    uint8_t                       const * p_application_data;
} pm_peer_data_unit_const_t;


/**@brief Data from @ref pm_peer_data_unit_t, as mapped in flash.
 */
typedef union
{
    pm_peer_data_bonding_t              const * p_bonding_data;
    bool                                const * p_service_changed_pending;
    pm_peer_data_local_gatt_db_flash_t  const * p_local_gatt_db;
    pm_peer_data_remote_gatt_db_flash_t const * p_remote_gatt_db;
    uint8_t                             const * p_application_data;
} pm_peer_data_unit_flash_t;


/**@brief The different types of data associated with a peer.
 */
typedef enum
{
    PM_PEER_DATA_ID_BONDING,
    PM_PEER_DATA_ID_SERVICE_CHANGED_PENDING,
    PM_PEER_DATA_ID_GATT_LOCAL,
    PM_PEER_DATA_ID_GATT_REMOTE,
    PM_PEER_DATA_ID_APPLICATION,
    PM_PEER_DATA_ID_INVALID,
} pm_peer_data_id_t;


//STATIC_ASSERT_MSG(sizeof(pm_peer_data_t) == sizeof(pm_peer_data_const_t), "Size of pm_peer_data_t different from immutable version.");


/**@brief Macro saying whether a data_id is valid, i.e. one of the valid enum values.
 *
 * @param[in] data_id  The data_id to check.
 */
static __INLINE bool PM_PEER_DATA_ID_IS_VALID(pm_peer_data_id_t data_id)
{
    return ((data_id - PM_PEER_DATA_ID_BONDING) < (PM_PEER_DATA_ID_INVALID - PM_PEER_DATA_ID_BONDING));
}


/**@brief One piece of data associated with a peer, together with the type.
 */
typedef struct
{
    uint16_t            length_words;
    pm_peer_data_id_t   data_type;
    pm_peer_data_unit_t data;
} pm_peer_data_t;

/**@brief Immutable version of @ref pm_peer_data_t.
 */
typedef struct
{
    uint16_t                    length_words;
    pm_peer_data_id_t           data_type;
    pm_peer_data_unit_const_t   data;
} pm_peer_data_const_t;

/**@brief Data from @ref pm_peer_data_t, as mapped in flash.
 */
typedef struct
{
    uint16_t                  length_words;
    pm_peer_data_id_t         data_type;
    pm_peer_data_unit_flash_t data;
} pm_peer_data_flash_t;


/**@brief Typedef for type used for write prepares. Used to reserve space in flash
 */
typedef uint32_t pm_prepare_token_t;


/**@brief Typedef for type used to hold reference to stored item in flash.
 *        this token can be used for locking and validity check
 */
typedef uint32_t pm_store_token_t;

 /** @} */

#endif /* PEER_MANAGER_TYPES_H__ */
