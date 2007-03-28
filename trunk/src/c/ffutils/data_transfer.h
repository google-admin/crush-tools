/** @file data_transfer.h
  * @brief contains data types and definitions specific to data transfer
  * files.
  */

#include <linklist.h>

#ifndef DATA_TRANSFER_H
#define DATA_TRANSFER_H

/** @brief label for the Time field */
#define DT_HEADER_TIME              "Time"
/** @brief label for the User-ID field */
#define DT_HEADER_USER_ID           "User-ID"
/** @brief label for the IP field */
#define DT_HEADER_IP                "IP"
/** @brief label for the Advertiser-ID field */
#define DT_HEADER_ADVERTISER_ID     "Advertiser-ID"
/** @brief label for the Buy-ID field */
#define DT_HEADER_BUY_ID            "Buy-ID"
/** @brief label for the Ad-ID field */
#define DT_HEADER_AD_ID             "Ad-ID"
/** @brief label for the Creative-ID field */
#define DT_HEADER_CREATIVE_ID       "Creative-ID"
/** @brief label for the Site-ID field */
#define DT_HEADER_SITE_ID           "Site-ID"
/** @brief label for the Page-ID field */
#define DT_HEADER_PAGE_ID           "Page-ID"
/** @brief label for the Site-Data field */
#define DT_HEADER_SITE_DATA         "Site-Data"
/** @brief label for the Activity-Type field */
#define DT_HEADER_ACTIVITY_TYPE     "Activity-Type"
/** @brief label for the Activity-Sub-Type field */
#define DT_HEADER_ACTIVITY_SUB_TYPE "Activity-Sub-Type"
/** @brief label for the Quantity field */
#define DT_HEADER_QUANTITY          "Quantity"
/** @brief label for the Revenue field */
#define DT_HEADER_REVENUE           "Revenue"
/** @brief label for the Local-User-ID field */
#define DT_HEADER_LOCAL_USER_ID     "Local-User-ID"
/** @brief label for the Transaction-ID field */
#define DT_HEADER_TRANSACTION_ID    "Transaction-ID"
/** @brief label for the Other-Data field */
#define DT_HEADER_OTHER_DATA        "Other-Data"
/** @brief label for the Ordinal field */
#define DT_HEADER_ORDINAL           "Ordinal"
/** @brief label for the Click-Time field */
#define DT_HEADER_CLICK_TIME        "Click-Time"
/** @brief label for the Event-ID field */
#define DT_HEADER_EVENT_ID          "Event-ID"

/** @brief Event-ID value of an unmatched activity */
#define DT_EVENT_ID_UNMATCHED  0
/** @brief Event-ID value of a post-click activity */
#define DT_EVENT_ID_POST_CLICK 1
/** @brief Event-ID value of a post-impression activity */
#define DT_EVENT_ID_POST_IMP   2

/** @brief maximum length of a type= value */
#define SPOTLIGHT_TYPE_MAX_LEN 8
/** @brief maximum length of a cat= value */
#define SPOTLIGHT_CAT_MAX_LEN  8

/** @brief spotlight group classification of sales vs. counter.
  */
typedef enum _spotlight_group_type {
		/** @brief counter tag group */
		spotlight_group_type_counter = 1,

		/** @brief sales tag group */
		spotlight_group_type_sales   = 2
	} spotlight_group_type_t;

/** @brief spotlight tag classification of the type of counter or sales
  * data recorded.
  */
typedef enum _spotlight_method {
		/** @brief count once per tag impression - ord=[rand] */
		spotlight_method_standard    = 1,

		/** @brief unique users for 24-hour period - ord=1 */
		spotlight_method_unique      = 2,

		/** @brief usually session counter - ord=[session-id] */
		spotlight_method_custom      = 3,

		/** @brief sales transaction - hardcoded qty=1 */
		spotlight_method_transaction = 4,

		/** @brief sales items - qty=[N] */
		spotlight_method_item        = 5

	} spotlight_method_t;

struct spotlight_tag_attributes {
	unsigned int spot_id;
	char type[SPOTLIGHT_TYPE_MAX_LEN + 1];
	char cat[SPOTLIGHT_TYPE_MAX_LEN + 1];
	spotlight_group_type_t group_type;
	spotlight_method_t     tag_method;
};

/** @brief gets attributes for all spotlight tags configured for a
  * spotlight id.
  *
  * note that database connection or query errors are fatal.
  * 
  * @param target  initialized linked list to hold the tag attributes
  * @param spot_id the spotlight configuration id in DART.
  * @param db_name database instance name (i.e. "PROD" or "DW") containing
  *			spot_activity_group and spot_activity tables
  * @param db_uid  database username
  * @param db_pass database password
  * 
  * @return the number of spotlight tags retrieved from the database.
  */
int get_spot_tag_attributes (
		llist_t *target,
		int spot_id,
		char *db_name,
		char *db_uid,
		char *db_pass
	);

#endif /* DATA_TRANSFER_H */

