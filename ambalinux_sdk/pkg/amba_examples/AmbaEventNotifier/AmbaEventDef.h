#ifndef __AMBAEVENTDEFINITION_H__
#define __AMBAEVENTDEFINITION_H__

#ifdef  __cplusplus
extern "C" {
#endif

#define EVENTNOTIFIER_RTOSEVENT(x) (0x00000000|x)
#define EVENTNOTIFIER_LINUXEVENT(x) (0x10000000|x)
#define EVENTNOTIFIER_RTOSERROR(x) (0xF00000000|x)
#define EVENTNOTIFIER_LINUXERROR(x) (0xF10000000|x)

/**
 * RTOS event type
 */
typedef enum _EVENTNOTIFIER_RTOSEVENT_e_ {
    EVENTNOTIFIER_RTOSEVENT_CARD_INSERT = EVENTNOTIFIER_RTOSEVENT(1), /**< card inserted at RTOS. */
    EVENTNOTIFIER_RTOSEVENT_CARD_REMOVE, /**< card remoted at RTOS. */
    EVENTNOTIFIER_RTOSEVENT_CARD_FORMAT, /**< card formated at RTOS. */
    EVENTNOTIFIER_RTOSEVENT_FILE_CREATE, /**< new file created at RTOS. */
    EVENTNOTIFIER_RTOSEVENT_FILE_DELETE, /**< file deleted at RTOS. */

    EVENTNOTIFIER_RTOSEVENT_GENERAL_ERROR = EVENTNOTIFIER_RTOSERROR(1), /**< General Error */
} EVENTNOTIFIER_RTOSEVENT_e;

/**
 * Linux event type
 */
typedef enum _EVENTNOTIFIER_LINUXEVENT_e_ {
    EVENTNOTIFIER_LINUXEVENT_BOOTED = EVENTNOTIFIER_LINUXEVENT(1), /**< Linux boot done. */
    EVENTNOTIFIER_LINUXEVENT_NETWORK_READY, /**< Network ready */
    EVENTNOTIFIER_LINUXEVENT_NETWORK_OFF, /**< Network turned off */
    EVENTNOTIFIER_LINUXEVENT_TAKEPHOTO,
    EVENTNOTIFIER_LINUXEVENT_RECORD,

    EVENTNOTIFIER_LINUXEVENT_GENERAL_ERROR = EVENTNOTIFIER_LINUXERROR(1), /**< General Error */
} EVENTNOTIFIER_LINUXEVENT_e;

#ifdef  __cplusplus
}
#endif
#endif /* __AMBAEVENTDEFINITION_H__ */

