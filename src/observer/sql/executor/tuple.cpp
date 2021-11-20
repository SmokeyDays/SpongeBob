/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2021/5/14.
//

#include "sql/executor/tuple.h"
#include "storage/common/table.h"
#include "storage/default/default_handler.h"
#include "common/log/log.h"

Tuple::Tuple(const Tuple &other) {
  LOG_PANIC("Copy constructor of tuple is not supported");
  exit(1);
}

Tuple::Tuple(Tuple &&other) noexcept : values_(std::move(other.values_)) {
}

Tuple & Tuple::operator=(Tuple &&other) noexcept {
  if (&other == this) {
    return *this;
  }

  values_.clear();
  values_.swap(other.values_);
  return *this;
}

Tuple::~Tuple() {
}

// add (Value && value)
void Tuple::add(TupleValue *value) {
  values_.emplace_back(value);
}
void Tuple::add(const std::shared_ptr<TupleValue> &other) {
  values_.emplace_back(other);
}
void Tuple::add(int value) {
  add(new IntValue(value));
}

void Tuple::add(float value) {
  add(new FloatValue(value));
}

void Tuple::add(const char *s, int len) {
  add(new StringValue(s, len));
}

void Tuple::add(const Tuple &other) {
  values_.insert(values_.begin(), other.values().begin(), other.values().end());
}

////////////////////////////////////////////////////////////////////////////////

std::string TupleField::to_string() const {
  return std::string(table_name_) + "." + field_name_ + std::to_string(type_);
}

////////////////////////////////////////////////////////////////////////////////
void TupleSchema::from_table(const Table *table, TupleSchema &schema) {
  const char *table_name = table->name();
  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num();
  for (int i = 0; i < field_num; i++) {
    const FieldMeta *field_meta = table_meta.field(i);
    if (field_meta->visible()) {
      schema.add(field_meta->type(), table_name, field_meta->name());
    }
  }
}

void TupleSchema::add(AttrType type, const char *table_name, const char *field_name) {
  fields_.emplace_back(type, table_name, field_name);
}

void TupleSchema::add_if_not_exists(AttrType type, const char *table_name, const char *field_name) {
  for (const auto &field: fields_) {
    if (0 == strcmp(field.table_name(), table_name) &&
        0 == strcmp(field.field_name(), field_name)) {
      return;
    }
  }

  add(type, table_name, field_name);
}

void TupleSchema::append(const TupleSchema &other) {
  fields_.reserve(fields_.size() + other.fields_.size());
  for (const auto &field: other.fields_) {
    fields_.emplace_back(field);
  }
}

int TupleSchema::index_of_field(const char *table_name, const char *field_name) const {
  const int size = fields_.size();
  for (int i = 0; i < size; i++) {
    const TupleField &field = fields_[i];
    if (0 == strcmp(field.table_name(), table_name) && 0 == strcmp(field.field_name(), field_name)) {
      return i;
    }
  }
  return -1;
}

void TupleSchema::print(std::ostream &os, int table_num) const {
  if (fields_.empty()) {
    os << "No schema";
    return;
  }

  // 判断有多张表还是只有一张表
  std::set<std::string> table_names;
  for (const auto &field: fields_) {
    table_names.insert(field.table_name());
  }
  printf("Print Table Num: %d, %d\n", table_num, table_names.size());
  for (std::vector<TupleField>::const_iterator iter = fields_.begin(), end = --fields_.end();
       iter != end; ++iter) {
    if (table_num > 1) {
      os << iter->table_name() << ".";
    }
    os << iter->field_name() << " | ";
  }

  if (table_num > 1) {
    os << fields_.back().table_name() << ".";
  }
  os << fields_.back().field_name() << std::endl;
}

/////////////////////////////////////////////////////////////////////////////
TupleSet::TupleSet(TupleSet &&other) : tuples_(std::move(other.tuples_)), schema_(other.schema_){
  other.schema_.clear();
}

TupleSet &TupleSet::operator=(TupleSet &&other) {
  if (this == &other) {
    return *this;
  }

  schema_.clear();
  schema_.append(other.schema_);
  other.schema_.clear();

  tuples_.clear();
  tuples_.swap(other.tuples_);
  return *this;
}

void TupleSet::add(Tuple &&tuple) {
  tuples_.emplace_back(std::move(tuple));
}

void TupleSet::multiply(TupleSet &&tuple_set) {
  schema_.append(tuple_set.get_schema());

  std::vector<Tuple> tuples;
  for(int i=0; i<tuples_.size(); ++i) {
    for(int j=0; j<tuple_set.tuples().size(); ++j) {
      Tuple tuple;
      tuple.add(tuple_set.tuples()[j]);
      tuple.add(tuples_[i]);
      tuples.emplace_back(std::move(tuple));
    }
  }
  tuples_.clear();
  for(int i=0; i<tuples.size(); ++i) {
    tuples_.emplace_back(std::move(tuples[i]));
  }
}

TupleValue * tupleValueFactory(const Value &value) {
  switch(value.type) {
    case CHARS: {
      
    }
    break;
    case INTS: {

    }
    break;
    case FLOATS: {

    }
    break;
    case DATES: {

    }
    break;
  }
}

bool filterTuple(const char *db, const Condition & condition, const Tuple & tuple, const TupleSchema & tuple_schema) {
  const char *left_value = nullptr;
  const char *right_value = nullptr;
  if( condition.left_is_attr ) {
    left_value = strdup(tuple.get(tuple_schema.index_of_field(condition.left_attr.relation_name, condition.left_attr.attribute_name)).get_value_string().c_str());
  } else {
    left_value = (char *)condition.left_value.data;
  }
  puts(left_value);
  if( condition.right_is_attr ) {
    right_value = strdup(tuple.get(tuple_schema.index_of_field(condition.right_attr.relation_name, condition.right_attr.attribute_name)).get_value_string().c_str());
  } else {
    right_value = (char *)condition.right_value.data;
  }


  AttrType type_left = UNDEFINED;
  AttrType type_right = UNDEFINED;

  if (1 == condition.left_is_attr) {
    const FieldMeta *field_left = 
      DefaultHandler::get_default().find_table(db, condition.left_attr.relation_name)->table_meta().field(condition.left_attr.attribute_name);
    type_left = field_left->type();
  } else {
    type_left = condition.left_value.type;
  }

  if (1 == condition.right_is_attr) {
    const FieldMeta *field_right = 
      DefaultHandler::get_default().find_table(db, condition.right_attr.relation_name)->table_meta().field(condition.right_attr.attribute_name);
    type_right = field_right->type();
  } else {
    type_right = condition.right_value.type;
  }

  int cmp_result = 0;
  switch (type_left)
  {
    case DATES:
    case CHARS: { // 字符串都是定长的，直接比较
      // 按照C字符串风格来定
      cmp_result = strcmp(left_value, right_value);
    }
    break;
    case INTS: {
      // 没有考虑大小端问题
      // 对int和float，要考虑字节对齐问题,有些平台下直接转换可能会跪
      int left = string_to_int(left_value);
      int right = string_to_int(right_value);
      cmp_result = left - right;
    }
    break;
    break;
    case FLOATS: {
      float left = string_to_float(left_value);
      float right = string_to_float(right_value);
      cmp_result = (int)(left - right);
    }
    break;
    default: {}
  }
  switch (condition.comp)
  {
  case EQUAL_TO:
    return 0 == cmp_result;
  case LESS_EQUAL:
    return cmp_result <= 0;
  case NOT_EQUAL:
    return cmp_result != 0;
  case LESS_THAN:
    return cmp_result < 0;
  case GREAT_EQUAL:
    return cmp_result >= 0;
  case GREAT_THAN:
    return cmp_result > 0;

  default:
    break;
  }

  LOG_PANIC("Never should print this.");
  return cmp_result;
}

RC TupleSet::filter(const char *db, const Selects & select) {
  // printf("Tuple size before filter: %d\n",tuples_.size());
  std::queue<int> to_delete;
  for(int i = select.condition_num - 1; i >= 0; --i) {
    // printf("Check condition %d\n",i);
    if( ( 0 == select.conditions[i].left_is_attr || 0 == select.conditions[i].right_is_attr ) ||
      (select.conditions[i].left_is_attr && (-1 == schema_.index_of_field(select.conditions[i].left_attr.relation_name, select.conditions[i].left_attr.attribute_name))) ||
      (select.conditions[i].right_is_attr && (-1 == schema_.index_of_field(select.conditions[i].right_attr.relation_name, select.conditions[i].right_attr.attribute_name)))
    ){
      continue;
    }
    for(int j = tuples_.size() - 1; j >= 0; --j) {
      // printf("Check Tuple: %d\n",j);
      if(!filterTuple(db, select.conditions[i], tuples_[j], schema_)){
        to_delete.push(j);
      }
    }
    while(!to_delete.empty()){
      // printf("Tuple size before erase: %d, erase: %d\n",tuples_.size(), to_delete.front());
      tuples_.erase(tuples_.begin() + to_delete.front());
      // printf("Tuple size after erase: %d\n",tuples_.size());
      to_delete.pop();
    }
  }
  printf("Tuple size after filter: %d\n",tuples_.size());
  return RC::SUCCESS;
}

void TupleSet::clear() {
  tuples_.clear();
  schema_.clear();
}

void TupleSet::print(std::ostream &os, int table_num) const {
  if (schema_.fields().empty()) {
    LOG_WARN("Got empty schema");
    return;
  }

  schema_.print(os, table_num);

  for (const Tuple &item : tuples_) {
    const std::vector<std::shared_ptr<TupleValue>> &values = item.values();
    for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = --values.end();
          iter != end; ++iter) {
      (*iter)->to_string(os);
      os << " | ";
    }
    values.back()->to_string(os);
    os << std::endl;
  }
}

int string_to_int (std::string str) {
  int v=0, l=str.length();
  if(str[0]=='-') for(int i=1;i<=l-1;++i) v=v*10+(str[i]-'0');
  else for(int i=0;i<=l-1;++i) v=v*10+(str[i]-'0');
  return str[0]=='-'?-v:v;
}
float string_to_float (std::string str) { //保证小数点两位
  float v=0, l=str.length();
  if(str[0]=='-') for(int i=1;i<=l-1-3;++i) v=v*10+(str[i]-'0');
  else for(int i=0;i<=l-1-3;++i) v=v*10+(str[i]-'0');
  v+=0.1*(str[l-1-1]-'0')+0.01*(str[l-1]-'0');
  return str[0]=='-'?-v:v;
}
std::string int_to_string (int v) {
  char c[100]; 
  sprintf(c, "%d", v);
  return c;
}
std::string float_to_string (float v) { //保留两位小数
  char c[100]; 
  sprintf(c, "%.2f", v);
  int len=strlen(c);
  if(c[len-1]=='0') {
    c[len-1]=c[len];
    if(c[len-2]=='0') c[len-3]=c[len];
  }
  return c;
}

const char *AGGR_TYPE_NAME[] = {
  "undefined",
  "MAX",
  "MIN",
  "COUNT",
  "AVG"
};

void TupleSet::print_aggregation(std::ostream &os, const Selects &selects) const {
  if (schema_.fields().empty()) {
    LOG_WARN("Got empty schema");
    return;
  }

  std::map<std::string, int>field_ids;
  std::vector<std::string> field_names;
  std::vector<AttrType> field_types;
  int id=0;
  for (const auto &field: schema_.fields()) {
    field_names.push_back(field.field_name());
    field_types.push_back(field.type());
    field_ids[field.field_name()] = id++;
  }

  std::vector<std::string>field_content[20];//SPONGEBOB
  for (const Tuple &item : tuples_) {
    int cnt=0; //列数
    const std::vector<std::shared_ptr<TupleValue>> &values = item.values();
    for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = values.end();
          iter != end; ++iter, ++cnt) {
      field_content[cnt].push_back((*iter)->get_value_string());
    }
  }

  for (int i=selects.aggre_num - 1; i>=0; --i) {
    os << AGGR_TYPE_NAME[selects.aggres[i].aggre_type] << "(" << selects.aggres[i].aggre_field_name << ")";
    if(0 != i) {
      os << " | ";
    }
  }
  os << "\n";

  for (int i=selects.aggre_num - 1; i>=0; --i){
    std::string field_name = selects.aggres[i].aggre_field_name;
    AggreType aggre_type = selects.aggres[i].aggre_type;
    int field_id = field_ids[field_name];
    AttrType field_type = field_types[field_id];
    switch (aggre_type){
      case MAX: 
      case MIN: {
        switch (field_type) {
          case INTS: {
            int ans = string_to_int(field_content[field_id][0]);
            for (auto v: field_content[field_id]) {
              int vnow = string_to_int(v);
              if (aggre_type == MIN ? (vnow < ans) : (vnow > ans)) ans = vnow;
            }
            os << int_to_string(ans);
            break;
          }
          case CHARS: {
            std::string ans = field_content[field_id][0];
            for (auto v: field_content[field_id]) {
              if (aggre_type == MIN ? (v < ans) : (v > ans)) ans = v;
            }
            os << ans;
            break;
          }
          case DATES: {
            std::string ans = field_content[field_id][0];
            for (auto v: field_content[field_id]) {
              if (aggre_type == MIN ? (v < ans) : (v > ans)) ans = v;
            }
            os << ans;
            break;
          }
          case FLOATS: {
            float ans = string_to_float(field_content[field_id][0]);
            for (auto v: field_content[field_id]) {
              float vnow = string_to_float(v);
              if (aggre_type == MIN ? (vnow < ans) : (vnow > ans)) ans = vnow;
            }
            os << float_to_string(ans);
            break;
          }
        }
        break;
      }
      case COUNT: {
        os << int_to_string(field_content[field_id].size());//SPONGEBOB: 没有 NULL
        break;
      }
      case AVG: {
        float avg = 0;
        for (auto v: field_content[field_id]) {
          avg += (field_type==INTS ? string_to_int(v) : string_to_float(v));
        }
        os << float_to_string(avg / field_content[field_id].size());
        break;
      }
      default:
        LOG_ERROR("SPONGEBOB!!!");
    }
    if (i!=0) os << " | ";
  }
  os << "\n";

  field_ids.clear();
  field_names.clear();
  field_types.clear();
  field_content->clear();//这个能直接用啊
}

void TupleSet::set_schema(const TupleSchema &schema) {
  schema_ = schema;
}

const TupleSchema &TupleSet::get_schema() const {
  return schema_;
}

bool TupleSet::is_empty() const {
  return tuples_.empty();
}

int TupleSet::size() const {
  return tuples_.size();
}

const Tuple &TupleSet::get(int index) const {
  return tuples_[index];
}

const std::vector<Tuple> &TupleSet::tuples() const {
  return tuples_;
}

/////////////////////////////////////////////////////////////////////////////
TupleRecordConverter::TupleRecordConverter(Table *table, TupleSet &tuple_set) :
      table_(table), tuple_set_(tuple_set){
}

void TupleRecordConverter::add_record(const char *record) {
  const TupleSchema &schema = tuple_set_.schema();
  Tuple tuple;
  const TableMeta &table_meta = table_->table_meta();
  for (const TupleField &field : schema.fields()) {
    const FieldMeta *field_meta = table_meta.field(field.field_name());
    assert(field_meta != nullptr);
    switch (field_meta->type()) {
      case INTS:{
        int value = *(int*)(record + field_meta->offset());
        tuple.add(value);
      }
      break;
      case DATES:{
        int value = *(int*)(record + field_meta->offset());
        char buf[16] = {0};
        snprintf(buf,sizeof(buf),"%04d-%02d-%02d",value/10000,    (value%10000)/100,value%100); // 注意这里月份和天数，不足两位时需要填充0
        tuple.add(buf,strlen(buf));
      }
      break;
      case FLOATS: {
        float value = *(float *)(record + field_meta->offset());
        tuple.add(value);
      }
        break;
      case CHARS: {
        const char *s = record + field_meta->offset();  // 现在当做Cstring来处理
        tuple.add(s, strlen(s));
      }
      break;
      default: {
        LOG_PANIC("Unsupported field type. type=%d", field_meta->type());
      }
    }
  }

  tuple_set_.add(std::move(tuple));
}


