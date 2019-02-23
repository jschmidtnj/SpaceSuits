export const state = () => ({
  notifications: []
})

export const mutations = {
  addNotification(state, data) {
    state.notifications.push({
      code: data.code,
      message: data.message
    })
  }
}

export const getters = {
  notifications: state => state.notifications
}
