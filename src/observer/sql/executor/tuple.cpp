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

void TupleSchema::print(std::ostream &os) const {
  if (fields_.empty()) {
    os << "No schema";
    return;
  }

  // 判断有多张表还是只有一张表
  std::set<std::string> table_names;
  for (const auto &field: fields_) {
    table_names.insert(field.table_name());
  }

  for (std::vector<TupleField>::const_iterator iter = fields_.begin(), end = --fields_.end();
       iter != end; ++iter) {
    if (table_names.size() > 1) {
      os << iter->table_name() << ".";
    }
    os << iter->field_name() << " | ";
  }

  if (table_names.size() > 1) {
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

void TupleSet::clear() {
  tuples_.clear();
  schema_.clear();
}

void TupleSet::print(std::ostream &os) const {
  if (schema_.fields().empty()) {
    LOG_WARN("Got empty schema");
    return;
  }

  schema_.print(os);

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
  for(int i=l-1;i>=0;--i) v=v*10+str[i]-'0';
  return v;
}
float string_to_float (std::string str) { //保证小数点两位
  float v=0, l=str.length();
  for(int i=l-1-3;i>=0;--i) v=v*10+(str[i]-'0');
  v+=0.1*(str[l-1-1]-'0')+0.01*(str[l-1]-'0');
  return v;
}
std::string int_to_string (int v) {
  char c[100]; //SPONGEBOB: VERY BAD
  sprintf(c, "%d", v);
  return c;
}
std::string float_to_string (float v) { //保留两位小数
  char c[100]; //SPONGEBOB: VERY BAD
  sprintf(c, "%.2f", v);
  return c;
}
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


