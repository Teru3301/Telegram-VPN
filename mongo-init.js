db = db.getSiblingDB('${MONGO_DB}');
db.createUser({
  user: '${MONGO_USER}',
  pwd: '${MONGO_PASS}',
  roles: [{ role: "readWrite", db: '${MONGO_DB}' }]
});

